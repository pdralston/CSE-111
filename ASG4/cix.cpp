// $Id: cix.cpp,v 1.9 2019-04-05 15:04:28-07 - - $

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
using namespace std;

#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>

#include "protocol.h"
#include "logstream.h"
#include "sockets.h"

logstream outlog (cout);
struct cix_exit: public exception {};

//The list of all client commands
unordered_map<string,cix_command> command_map {
   {"exit", cix_command::EXIT},
   {"help", cix_command::HELP},
   {"ls"  , cix_command::LS  },
   {"put" , cix_command::PUT },
   {"get" , cix_command::GET },
   {"rm"  , cix_command::RM  }
};

static const char help[] = R"||(
exit         - Exit the program.  Equivalent to EOF.
get filename - Copy remote file to local host.
help         - Print help summary.
ls           - List names of files on remote server.
put filename - Copy local file to remote host.
rm filename  - Remove file from remote server.
)||";

void cix_help() {
   cout << help;
}

void cix_ls (client_socket& server) {
   cix_header header;
   header.command = cix_command::LS;
   outlog << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   outlog << "received header " << header << endl;
   if (header.command != cix_command::LSOUT) {
      outlog << "sent LS, server did not return LSOUT" << endl;
      outlog << "server returned " << header << endl;
   }else {
      auto buffer = make_unique<char[]> (header.nbytes + 1);
      recv_packet (server, buffer.get(), header.nbytes);
      outlog << "received " << header.nbytes << " bytes" << endl;
      buffer[header.nbytes] = '\0';
      cout << buffer.get();
   }
}

//GET command - gets a file from the server
void cix_get (client_socket& server, string filename){
   if(filename.find("/") != string::npos) {
      outlog << "passed directory instead of file" << endl;
      return;
   }
   else if(filename.size() > 58) {
     outlog << "passed file with filename > 58 characters." << endl;
     return;
   }

   cix_header header;
   header.command = cix_command::GET;
   strcpy(header.filename, filename.c_str());
   outlog << "getting file " << header.filename << endl;
   outlog << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   outlog << "received header " << header << endl;
   if (header.command != cix_command::FILEOUT) {
     outlog << "sent GET, server did not return FILEOUT" << endl;
     outlog << "server returned " << header << endl;
   }
   else {
     auto buffer = make_unique<char[]> (header.nbytes + 1);
     recv_packet (server, buffer.get(), header.nbytes);
     outlog << "received " << header.nbytes << " bytes" << endl;
     buffer[header.nbytes] = '\0';
     ofstream outfile {static_cast<string>(header.filename)};
     if (!outfile.good()) {
       outlog << "Error creating file " <<
          filename << endl;
     }
     if (header.nbytes > 0) {
        outfile << buffer.get();
     }
     outfile.close();
   }
}

//PUT command - creates a file in the server with contents
//that are passed.
void cix_put (client_socket& server, string filename) {
   if(filename.find("/") != string::npos) {
      outlog << "passed directory instead of file" << endl;
      return;
   }
   else if(filename.size() > 58) {
      outlog << "passed file with filename > 58 characters." << endl;
      return;
   }

   ifstream infile (filename);
   if(!infile.good()) {
      outlog << "file doesn't exist locally." << endl;
      return;
   }

   cix_header header;

   infile.seekg(0, infile.end);
   const int content_size = infile.tellg();
   infile.seekg(0, infile.beg);
   unique_ptr<char[]> contents(new char[content_size]);
   infile.read(contents.get(), content_size);
   infile.close();


   header.command = cix_command::PUT;
   strcpy(header.filename, filename.c_str());
   header.nbytes = content_size;
   outlog << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   send_packet (server, contents.get(), content_size);
   recv_packet (server, &header, sizeof header);
   outlog << "received header " << header << endl;
   if (header.command != cix_command::ACK) {
      outlog << "sent GET, server did not return ACK" << endl;
      outlog << "server returned " << header << endl;
   }
   else {
      outlog << "sent GET, server returned ACK" << endl;
      outlog << "file successfully added" << endl;
   }
}

//RM command - removes a file in the server with name
//that is passed
void cix_rm (client_socket& server, string filename) {
   if(filename.find("/") != string::npos) {
      outlog << "passed directory instead of file" << endl;
      return;
   }
   else if(filename.size() > 58) {
      outlog << "passed file with filename > 58 characters." << endl;
      return;
   }

   cix_header header;
   header.command = cix_command::RM;
   outlog << "sending header " << header << endl;
   strcpy(header.filename, filename.c_str());
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   outlog << "received header " << header << endl;
   if (header.command != cix_command::ACK) {
     outlog << "sent RM, server did not receive ACK" << endl;
     outlog << "server returned " << header << endl;
   }
   else {
     outlog << "sent RM, received ACK" << endl;
     outlog << "file succesfully removed" << endl;
   }
}

void usage() {
   cerr << "Usage: " << outlog.execname() << " [host] [port]" << endl;
   throw cix_exit();
}

int main (int argc, char** argv) {
   outlog.execname (basename (argv[0]));
   outlog << "starting" << endl;
   vector<string> args (&argv[1], &argv[argc]);
   if (args.size() > 2) usage();
   string host = get_cix_server_host (args, 0);
   in_port_t port = get_cix_server_port (args, 1);
   outlog << to_string (hostinfo()) << endl;
   try {
      outlog << "connecting to " << host << " port " << port << endl;
      client_socket server (host, port);
      outlog << "connected to " << to_string (server) << endl;
      for (;;) {
         string line;
         getline (cin, line);
         if (cin.eof()) throw cix_exit();
         outlog << "command " << line << endl;
         const auto& itor = command_map.find (line.substr(0, line.find(" ")));
         cix_command cmd = itor == command_map.end()
                         ? cix_command::ERROR : itor->second;
         string arguments = "";
         switch (cmd) {
            case cix_command::EXIT:
               throw cix_exit();
               break;
            case cix_command::HELP:
               cix_help();
               break;
            case cix_command::LS:
               cix_ls (server);
               break;
            case cix_command::GET:
              arguments = line.substr(4, line.size() - 1);
              cout << "arguments: " << arguments << endl;
              cix_get(server, arguments);
              break;
            case cix_command::RM:
              arguments = line.substr(3, line.size() - 1);
              cout << "arguments: " << arguments << endl;
              cix_rm(server, arguments);
              break;
            case cix_command::PUT:
              arguments = line.substr(4, line.size() - 1);
              cout << "arguments: " << arguments << endl;
              cix_put(server, arguments);
              break;
            default:
               outlog << line << ": invalid command" << endl;
               break;
         }
      }
   } catch (socket_error& error) {
      outlog << error.what() << endl;
   } catch (cix_exit& error) {
      outlog << "caught cix_exit" << endl;
   }
   outlog << "finishing" << endl;
   return 0;
}
