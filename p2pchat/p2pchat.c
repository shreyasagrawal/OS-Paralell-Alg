#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "socket.h"
#include "ui.h"

#define MESSAGE_LENGTH 256

// global lock
pthread_mutex_t lock;

// Keep the username in a global so we can access it from the callback
const char* username;

// Struct to hold the server socket file descriptor
typedef struct thread_arg {
  int server_socket_fd;
} thread_arg_t;

// struct to hold the client socket file descriptor for the newly connected client
typedef struct client_args {
  int client_socket_fd;
} client_args_t;

// Global client list array holding all the clients connected
int* client_list;

// count is the number of clients connected at a certain time
int count = 0;

// The send message function code below is from the networking excercise by Charlie Curtsinger
// Send a message across a socket with a header that includes the message length.
int send_message(int fd, const char* message) {
  // If the message is NULL, set errno to EINVAL and return an error
  if (message == NULL) {
    errno = EINVAL;
    return -1;
  }

  // First, send the length of the message in a size_t
  size_t len = strlen(message);
  if (write(fd, &len, sizeof(size_t)) != sizeof(size_t)) {
    // Writing failed, so return an error
    return -1;
  }

  // Now we can send the message. Loop until the entire message has been written.
  size_t bytes_written = 0;
  while (bytes_written < len) {
    // Try to write the entire remaining message
    ssize_t rc = write(fd, message + bytes_written, len - bytes_written);

    // Did the write fail? If so, return an error
    if (rc <= 0) return -1;

    // If there was no error, write returned the number of bytes written
    bytes_written += rc;
  }

  return 0;
}

// The receive message function code below is from the networking excercise by Charlie Curtsinger
// Receive a message from a socket and return the message string (which must be freed later)
char* receive_message(int fd) {
  // First try to read in the message length
  size_t len;
  if (read(fd, &len, sizeof(size_t)) != sizeof(size_t)) {
    // Reading failed. Return an error
    return NULL;
  }

  // Now make sure the message length is reasonable
  if (len > MESSAGE_LENGTH) {
    errno = EINVAL;
    return NULL;
  }

  // Allocate space for the message
  char* result = malloc(len + 1);

  // Try to read the message. Loop until the entire message has been read.
  size_t bytes_read = 0;
  while (bytes_read < len) {
    // Try to read the entire remaining message
    ssize_t rc = read(fd, result + bytes_read, len - bytes_read);

    // Did the read fail? If so, return an error
    if (rc <= 0) {
      free(result);
      return NULL;
    }

    // Update the number of bytes read
    bytes_read += rc;
  }

  result[len] = '\0';

  return result;
}

// This function is run whenever the user hits enter after typing a message
void input_callback(const char* message) {
  // This will trigger the client to exit from the network.
  if (strcmp(message, ":quit") == 0 || strcmp(message, ":q") == 0) {
    ui_exit();
  } 
  else {
    if (strlen(message) > MESSAGE_LENGTH) {
      ui_display("Warning: ", "You're trying to send a message whose size is greater than the maximum length allowed");
      return;
    }
    
    // A temp variable to hold the concatenated string username: message
    char temp[MESSAGE_LENGTH * 2];

    // Looping over the client list and sending a concatenated message with username
    for (int i = 0; i < count; i++) {

      // lock since modifying messages. Making this atomic to avoid any bugs
      pthread_mutex_lock(&lock);
      temp[(MESSAGE_LENGTH*2)-1] = '\0';
      strcpy(temp, username);
      strcat(temp, ":");
      strcat(temp, message);
      
      // sending the concatenated message across to all the clients
      int rc = send_message(client_list[i], temp);
      if (rc == -1) {
        perror("Failed to send message");
        exit(EXIT_FAILURE);
      } 
      pthread_mutex_unlock(&lock);
    }
    ui_display(username, message);
  }
}

// Each client has a talk thread which they use to talk to all other clients on the network
void* talk(void* args) {

  // Object of the client struct to find the client socket file descriptor for the new client
  client_args_t* p = (client_args_t*) args;
  bool client_alive = true;
  // new client
  int client_socket_fd = p->client_socket_fd;
  
  // To hold the index of the client to remove from the client list
  int index;

  // The thread spins as long as the client is active
  while (client_alive) {
    // Read a message from the client
    char* buffer = receive_message(client_socket_fd);
    

    // If no message is found then client has disconnected
    if (buffer == NULL) {
      // loop over to find out the index of the client that disconnected
      for (int i = 0; i < count; i++) {
        if (client_list[i] == client_socket_fd) index = i;
      }
      // using memmove to shift the entire client list array based on where the client who quit was in the list
      memmove(&client_list[index], &client_list[index + 1], sizeof(int)*(count-index-1));
      // decrement number of clients
      count--;
      // client has disconnected
      client_alive = false;
      continue;
    }
    buffer[MESSAGE_LENGTH-1] = '\0';
    // Send messages to everyone except yourself
    for (int i = 0; i<count; i++){
      if (client_list[i] != client_socket_fd){
        int rc = send_message(client_list[i], buffer);
      // error check
        if (rc == -1) {
          perror("Failed to send message");
          exit(EXIT_FAILURE);
        }
      }
    }
    
    // Use strtok to split the string by ':' 
    char * token = strtok(buffer, ":");
    if (token == NULL){
      perror("Nothing left to tokenize");
    }
    
    // Placeholder for holding the username after splitting
    char temp_username[MESSAGE_LENGTH] = "";
    temp_username[MESSAGE_LENGTH-1] = '\0';

    strcpy(temp_username,token);

    token = strtok(NULL, ":");
    if (token == NULL){
      perror("Nothing left to tokenize");
    }

    // Placeholder for holding the message after splitting
    char current_message[MESSAGE_LENGTH] = "";
    current_message[MESSAGE_LENGTH-1] = '\0';
    strcpy(current_message, token);

    ui_display(temp_username, current_message);
  }
  // At this point client has disconnected
  ui_display("Client disconnected", "");

  // CLosing
  close(client_socket_fd);

  return NULL;
}

// A constantly running thread to accept connections and add to the global client list
void * spinning_thread(void * args) {
  thread_arg_t* p = (thread_arg_t*) args;
  
  // Allocated 1 more space on the global list to hold the next client
  client_list = (int*) realloc(client_list, sizeof(int)* (count+1));

  // infinite loop to keep accepting
  while(1) {
    ui_display("Waiting for client", "");
    // Accept an incoming connection on a server socket.
    int client_fd = server_socket_accept(p->server_socket_fd);
    
    // Error check
    if (client_fd == -1) {
      perror("accept failed");
      exit(EXIT_FAILURE);
    }
    // add this client to our global list of clients
    pthread_mutex_lock(&lock);
    client_list[count] = client_fd;
    pthread_mutex_unlock(&lock);

    // create an arg that holds the current client's file descriptor so that it can be sent to the talk thread
    client_args_t *client_args = malloc(sizeof(client_args_t));
    client_args->client_socket_fd = client_list[count];

    // new client added
    count++;

    // At this point a new client has been connected
    ui_display("Client connected","");

    pthread_t id; 
    //create a thread for each client here.
    if (pthread_create(&id, NULL, talk, client_args)) {
      perror("pthread_create failed");
      exit(EXIT_FAILURE);
    }
  }
}

int main(int argc, char** argv) {

  // initialize the user interface
  ui_init(input_callback);

  // Make sure the arguments include a username
  if (argc != 2 && argc != 4) {
    fprintf(stderr, "Usage: %s <username> [<peer> <port number>]\n", argv[0]);
    exit(1);
  }

  // Save the username in a global
  username = argv[1];

  // Set up a server socket to accept connections, 
  // and setting port to 0 will make the program choose the port number
  unsigned short port = 0;
  int server_socket_fd = server_socket_open(&port);
  // error check
  if (server_socket_fd == -1) {
    perror("Server socket was not opened");
    exit(EXIT_FAILURE);
  }
  // Print out the port the server has open for connections
  char server_msg[256];
  sprintf(server_msg, "Server running on port number: %u", port);

  // listen on this server socket file descriptor
  if (listen(server_socket_fd, 1)) {
    perror("listen failed");
    exit(EXIT_FAILURE);
  }

  // Did the user specify a peer we should connect to?
  if (argc == 4) {
    // Unpack arguments
    char* peer_hostname = argv[2];
    unsigned short peer_port = atoi(argv[3]);

    // Create space to hold our clients
    client_list = (int*) malloc(sizeof(int));

    // Add connected client to our global list
    pthread_mutex_lock(&lock);
    client_list[count] = socket_connect(peer_hostname, peer_port);
    
    //error check
    if (client_list[count] == -1) {
      perror("failed to connect");
      exit(EXIT_FAILURE);
    }
    pthread_mutex_unlock(&lock);

    // create an arg that holds the current client's file descriptor so that it can be sent to the talk thread
    client_args_t *client_args = malloc(sizeof(client_args_t));
    client_args->client_socket_fd = client_list[count];

    // new client added
    count++;

    // Client has connected at this point
    ui_display("Client connected", "");
    pthread_t id;

    // New talk thread for this client
    if (pthread_create(&id, NULL, talk, client_args)) {
      perror("pthread_create failed");
      exit(EXIT_FAILURE);
    }
  }

  // Thread to accept connections infinitely in the background
  pthread_t accepting_thread;
  thread_arg_t args;
  args.server_socket_fd = server_socket_fd;
  if (pthread_create(&accepting_thread, NULL, spinning_thread, &args)) {
    perror("pthread_create failed");
    exit(EXIT_FAILURE);
  }


  // Once the UI is running, you can use it to display log messages
  ui_display("INFO", server_msg);

  // Run the UI loop. This function only returns once we call ui_stop() somewhere in the program.
  ui_run();

  return 0;
}