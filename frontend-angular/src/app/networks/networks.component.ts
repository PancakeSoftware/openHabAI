import {Component, Input, OnInit} from '@angular/core';
import {ActivatedRoute, Params} from "@angular/router";
import {Observable} from "rxjs/Observable";
import {toastInfo, toastOk} from "../util/Log";
import {ApiConnection} from "../util/Api/ApiConnection";
import {Api} from "../util/Api/Api";

@Component({
  selector: 'app-networks',
  templateUrl: './networks.component.html',
  styles: []
})
export class NetworksComponent implements OnInit {

  structureID: number = -1;
  networks: Observable<Network[]>;

  constructor(
    private router: ActivatedRoute,
    private api: Api)
  {
    router.params.subscribe(params => {
      if (this.structureID !== params.structureID) {
        this.structureID = params.structureID;
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
    this.networks = this.api.list(`/dataStructures/${this.structureID}/networks`).items();
  }
}

type Network = {name: string, id: number};