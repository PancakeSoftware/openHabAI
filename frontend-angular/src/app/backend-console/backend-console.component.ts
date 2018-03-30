import { Component, OnInit } from '@angular/core';
import {Backend, CONNECTION_STATUS} from "../util/backendSocket";
import {toastInfo} from "../util/Log";
import {delay} from "q";

@Component({
  selector: 'app-backend-console',
  templateUrl: './backend-console.component.html',
  styleUrls: ['./backend-console.components.css'],
})
export class BackendConsoleComponent implements OnInit {

  log: {direction: string, message: any}[] = [];
  sendText: string;
  connected: CONNECTION_STATUS = CONNECTION_STATUS.NOT_CONNECTED;
  CONNECTION_STATUS = CONNECTION_STATUS;


  constructor() {
    Backend.onReceive.subscribe(data => {
      this.log.push({direction: 'got', message: data});
    });
    Backend.onSend.subscribe(data => {
      this.log.push({direction: 'send', message: data});
    });

    // get status updates
    Backend.connectionStatus.subscribe(status => this.connected = status);
  }

  ngOnInit() {
  }

  onSendButton() {
    /*
    Backend.send(this.sendText);
    toastInfo(this.sendText);*/
  }
}
