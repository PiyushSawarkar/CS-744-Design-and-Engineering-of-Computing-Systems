#include "http_server.hh"
#include <vector>
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
vector<string> split(const string &s, char delim) {
	vector<string> elems;
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		if (!item.empty())
		elems.push_back(item);
	}
	return elems;
}
HTTP_Request::HTTP_Request(string request) {
	vector<string> lines = split(request, '\n');
	vector<string> first_line = split(lines[0], ' ');
	this->HTTP_version = "1.0"; // We'll be using 1.0 irrespective of the request
	/*
	TODO : extract the request method and URL from first_line here
	*/
	this->method=first_line[0];
	this->url=first_line[1];
	if (this->method != "GET") {
		cerr << "Method '" << this->method << "' not supported" << endl;
		exit(1);
	}
}
HTTP_Response *handle_request(string req) {
	HTTP_Request *request = new HTTP_Request(req);
	HTTP_Response *response = new HTTP_Response();
	string url = string("html_files")+request->url;
	response->HTTP_version = "1.0";
	struct stat sb;
	if (lstat(url.c_str(), &sb) == 0) // requested path exists 
	{
		response->status_code = "200";
		response->status_text = "OK";
		response->content_type = "text/html";
		string body;
		if (S_ISDIR(sb.st_mode)) {
			/*
			In this case, requested path is a directory.
			TODO : find the index.html file in that directory (modify the url
			accordingly)
			*/
		DIR *directory;
		struct dirent *x;
		string s="index.html";
		bool result=false;
		if ((directory=opendir(url.c_str()))!=NULL){
			while ((x = readdir (directory)) != NULL){
				if(s==x->d_name){
					result=true;
					break;
				}   
			}
		}
		closedir(directory);
		if(result)
				url=url+"/index.html";
		}
			/*
			TODO : open the file and read its contents
			*/
		ifstream f(url);
		if(f){
			ostringstream ss;
			ss << f.rdbuf();
			body = ss.str();
		}
			/*
			TODO : set the remaining fields of response appropriately
			*/
		response->body=body;
		response->content_length=to_string(body.size());    
  	}
	else {
			response->status_code = "404";
		/*
		TODO : set the remaining fields of response appropriately
		*/
			response->status_text = "Not Found";
			response->content_type = "text/html";
			response->body="<!DOCTYPE html><html><body><h1>Error 404<h1></body></html>";
			response->content_length=to_string(response->body.size());
	}
	delete request;
	return response;
}
string HTTP_Response::get_string() {
  /*
  TODO : implement this function
  */
	string line1="HTTP/"+HTTP_version+" "+status_code+" "+status_text;
	string line2="Content-Type: "+content_type;
	string line3="Content-Length: "+content_length;
	string line4=body;
	string response=line1+"\r\n"+line2+"\r\n"+line3+"\r\n\r\n"+line4;
	return response;
}
