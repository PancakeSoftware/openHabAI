#include <uWS.h>
#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include <thread>

using namespace std;

/*
 * MAIN
 * simple webSocket and http server
 * (single threaded)
 */
int main()
{
  using namespace boost;

  // cache all *.html
  map<string, string> webDocs;
  try
  {
    auto path = filesystem::path("./../frontend-angular/dist");
    for (filesystem::recursive_directory_iterator i(path), end; i != end; ++i)
    {
      if (!is_directory(i->path())) {
        string webDocPath = i->path().string().substr(path.size(), i->path().size() - path.size());
        cout << webDocPath << "\n";
        filesystem::ifstream infile{i->path()};
        string str{istreambuf_iterator<char>(infile), istreambuf_iterator<char>()};
        webDocs.emplace(webDocPath, str);
      }
    }
  } catch (std::exception &e) {
    cout << "can't read index.html: " << e.what() << endl;
  }


  uWS::Hub h;
  uWS::WebSocket<uWS::SERVER> *client1 = nullptr;

  h.onMessage([](uWS::WebSocket<uWS::SERVER> *ws, char *message, size_t length, uWS::OpCode opCode) {
    // on webSocket receive
    cout << "got webSocket-msg from   IP-addr: " << ws->getAddress().address << "     Port: " << ws->getAddress().port << "      opCode: " << opCode << "  msg: " << string(message).substr(0, length) << endl;
    ws->send(message, length, opCode);
  });

  h.onConnection([&client1](uWS::WebSocket<uWS::SERVER> *ws, uWS::HttpRequest request){
    // on webSocket new connection
    cout << "new webSocket-connection  IP-addr: " << ws->getAddress().address << "     Port: " << ws->getAddress().port << "    url:" << request.getUrl().toString() << endl;
    // send hello
    string msg = "hello from c++";
    //ws->send(msg.data(), msg.size(), uWS::TEXT /* send type: Text */);
    client1 = ws;
  });

  h.onDisconnection([](uWS::WebSocket<uWS::SERVER> *ws, int code, char *message, size_t length){
    // on webSocket connection lost
    cout << "webSocket-connection gone  IP-addr: " << ws->getAddress().address << "     Port: " << ws->getAddress().port << "  code:"<< code <<  "  msg:" << message << endl;
  });

  h.onHttpRequest([webDocs](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t, size_t) {
    // on Http request
    cout << "GOT http request:  url: " << req.getUrl().toString() << "   httpMethod: " << req.getMethod() << endl;

    auto doc = webDocs.find(req.getUrl().toString());
    if (doc != webDocs.end())
      res->end(doc->second.data(), doc->second.size());
    else {
      cout << "redirect from " << req.getUrl().toString() << " to index.html " << endl;
      doc = webDocs.find("/index.html");
      if (doc != webDocs.end())
        res->end(doc->second.data(), doc->second.size());
      else {
        string s = "can't find index.html :(";
        res->end(s.data(), s.size());
      }
    }
    /*
    if (req.getUrl().toString() == "/console") {
      res->end(indexHtml.data(), indexHtml.size());
    }
    else {
      string resStr = "Hello from c++ !\nTo open webSocket-console open url: http://localhost:3000/console\nyou try to get '" + req.getUrl().toString() + "'\nData: "
          + string(data == NULL ? "" : data);
      res->end(resStr.data(), resStr.size());
    }
     */
  });


  thread serverThread([&h](){
    if (h.listen(3000)) {
      // run is blocking
      h.run();
    }
  });

  cout << "will start loop" << endl;

  int i=0;
  while (true) {
    sleep(1);
    if (client1 != nullptr) {
      string msg = "hello: " + to_string(i);
      //cout << "send to " << client1->getAddress().address << endl;
      client1->send(msg.data(), msg.size(), uWS::TEXT);
    }

    i++;
  }

  serverThread.join();

  // on exit
  return EXIT_SUCCESS;
}