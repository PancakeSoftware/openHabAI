import { Component, OnInit } from '@angular/core';
import {ApiConnection, CONNECTION_STATUS} from "@catflow/ApiConnection";
import {SettingsService} from "@frontend/settings/settings.service";

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


  constructor(private settings:SettingsService) {
    ApiConnection.onReceive.subscribe(data => {
      this.log.push({direction: 'got', message: data});
      if (this.log.length > 50)
        this.log = this.log.slice(0, this.log.length - 50);
    });
    ApiConnection.onSend.subscribe(data => {
      this.log.push({direction: 'send', message: data});
      if (this.log.length > 50)
        this.log = this.log.slice(0, this.log.length - 50);
    });

    // get status updates
    ApiConnection.connectionStatus.subscribe(status => this.connected = status);
  }

  ngOnInit() {
  }

  onSendButton() {
    /*
    ApiConnection.send(this.sendText);
    toastInfo(this.sendText);*/
  }
}
