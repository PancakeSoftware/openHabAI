import {Component, Input, OnInit} from '@angular/core';
import {ActivatedRoute, Params} from "@angular/router";
import {Observable} from "rxjs/Observable";
import {toastInfo, toastOk} from "../util/Log";
import {Backend} from "../util/backendSocket";

@Component({
  selector: 'app-networks',
  templateUrl: './networks.component.html',
  styles: []
})
export class NetworksComponent implements OnInit {

  ID_DataStructure: number = -1;
  networks:
    {name: string, id: number}[] = [];

  constructor(private router: ActivatedRoute) {
    toastInfo('networks constructor');
    router.params.subscribe(params => {
      if (this.ID_DataStructure !== params.ID_DataStructure) {
        this.ID_DataStructure = params.ID_DataStructure;
        this.updateSelf();
      }
    });
  }

  ngOnInit() {
  }

  /* reload all
   */
  updateSelf() {
    // get data
    Backend.sendRequest([{"dataStructures": ""+this.ID_DataStructure}, {"networks": ""}], "getAll", (what, data) => {
      if (what == 'ok')
        this.networks = data;
    });
  }
}
