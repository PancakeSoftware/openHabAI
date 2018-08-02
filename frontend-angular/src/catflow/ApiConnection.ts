
// -- socket ------------------------------------------------------------
import {ReplaySubject} from "rxjs/ReplaySubject";
import {EventEmitter} from "@angular/core";
import {ApiRoute} from "./Utils";

export class ApiConnection {

  private static sock: WebSocket = null;
  private static respondCounter: number;
  private static respondCallbacks: any[] = [];
  private static actionCallbacks: any[] = [];
  private static sendQueue: {route, what: string, callback, data} [] = [];

  static onReceive = new ReplaySubject<any>();
  static onSend: ReplaySubject<any> = new ReplaySubject<any>();
  static onReady: EventEmitter<undefined> = new EventEmitter();
  static onGetError: ReplaySubject<{route: string, message: string}> = new ReplaySubject();
  static connectionStatus: ReplaySubject<CONNECTION_STATUS> = new ReplaySubject<CONNECTION_STATUS>();
  static onDisconnect: () => void = null;

  static connect(host: string, port: number) {
    // -- connection ------------------------------------------------------------
    console.info('will connect to: ' + 'ws://'+host+':'+port);

    if (this.sock != null) {
      this.sock.onerror = undefined;
      this.sock.onclose = undefined;
      this.sock.onopen = undefined;
      this.sock.onmessage = undefined;
    }

    this.sock = new WebSocket('ws://'+host+':'+port);
    this.sock.onerror = (error) => {
      console.error("can't connect to server");
      this.connectionStatus.next(CONNECTION_STATUS.NOT_CONNECTED);
    };

    this.sock.onclose = (ev: CloseEvent) => {
      if (this.onDisconnect != null)
        this.onDisconnect();
    };

    this.sock.onopen = (p1) => {
      console.info("connected with server OK");
      this.onReady.emit();
      this.connectionStatus.next(CONNECTION_STATUS.CONNECTED);

      // send all in queue
      this.progressQueue()
    };

    // message id
    this.respondCounter = 0;
    this.respondCallbacks.length = 0;

    this.sock.onmessage =  (event) => {
      //console.log(event.data);
      var data = JSON.parse(event.data);

      this.onReceive.next(JSON.stringify(data, null, 2));

      switch (data.type) {
        case "respond":
          if (data.what == "error") {
            this.onGetError.next({
              route: ((data.data.respondId < 0) ? data.data.request.route : data.data.route),
              message: data.data.message
            });
            return;
          }
          if (this.respondCallbacks[data.respondId] != undefined) {
            if (data.data !== undefined)
              this.respondCallbacks[data.respondId](data.what, data.data);
            else
              this.respondCallbacks[data.respondId](data.what);
            delete this.respondCallbacks[data.respondId];
          }
          break;

        case "request":
          if (this.actionCallbacks[data.route] != undefined) {
            this.actionCallbacks[data.route](data.what, data.data);
          } else
            console.warn("no callback function to progress request for route " + data.route);
          break;

        default:
          console.error("got unknown message type '" + data.type + "'");
      }
    }
  }

  static listenForAction(route: string, callback: (action: string, data: any) => void) {
    this.actionCallbacks[route] = callback;
  }

  static sendRequest(route: ApiRoute, what: string, callback = null, data = null) {
    this.sendQueue.push({route, what, callback, data});
    this.progressQueue();
  }

  static sendNonWait(data: string) {
    if (this.sock.readyState == this.sock.CLOSED || this.sock.readyState == this.sock.CONNECTING)
      return;

    this.onSend.next(data);
    this.sock.send(data);
  }

  private static progressQueue() {
    if (this.sock == undefined)
      return;
    if (this.sock.readyState == this.sock.OPEN) {
      while (this.sendQueue.length > 0) {
        let msg = this.sendQueue.pop();
        this.sendRequestNonWait(msg.route, msg.what, msg.callback, msg.data)
      }
    }
  }

  private static sendRequestNonWait(route: string, what: string, callback = null, data = null) {
    if (this.sock.readyState == this.sock.CLOSED || this.sock.readyState == this.sock.CONNECTING)
      return;

    var dataS = {
      "type": "request",
      "route": route,
      "what": what,
      "data": "",
      "respondId": -1
    };

    if ((data != null))
      dataS.data = data;

    if (callback !== undefined && callback !== null) {
      // want respond
      dataS.respondId = this.respondCounter;
      this.respondCallbacks[this.respondCounter] = callback;
      this.respondCounter++;
    }

    this.onSend.next(JSON.stringify(dataS, null, 2));
    this.sock.send(JSON.stringify(dataS));
  };
}

export enum CONNECTION_STATUS {
  NOT_CONNECTED,
  CONNECTED
}



