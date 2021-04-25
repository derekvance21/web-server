#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <vector>
#include <unordered_map>
#include <map>


class Request {
 public:
  Request(const std::string& request);

  /* Main Function */
  int ParseRequest();

  /* Helper Functions */
  int ExtractMethod();
  int ExtractPath();
  int ExtractVersion();
  int ExtractHeaders();
  int ExtractBody();

  /* Getters (for testing) */
  std::string GetRequest();
  std::string GetMethod();
  std::string GetPath();
  std::string GetVersion();
  std::string GetBody();
  std::unordered_map<std::string, std::string> GetHeaders();

  /* Setters (for testing) */
  void SetRequest(std::string request);
  void SetMethod(std::string method);
  void SetPath(std::string path);
  void SetVersion(std::string version);
  void SetBody(std::string body);
  void SetHeaders(std::unordered_map<std::string, std::string> headers);

 private:
  std::string request_;
  std::string method_;
  std::string path_;
  std::string version_;
  std::unordered_map<std::string, std::string> headers_;
  std::string body_;
};

#endif
