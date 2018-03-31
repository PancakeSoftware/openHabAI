import {Component, EventEmitter, OnInit} from '@angular/core';
import {ActivatedRoute, Params} from "@angular/router";
import {toastInfo} from "../util/Log";

@Component({
  selector: 'app-networks-and-train',
  templateUrl: './networks-and-train.component.html',
  styles: []
})
export class NetworksAndTrainComponent implements OnInit {

  networkID: number;
  structureID: number;

  constructor(private route: ActivatedRoute) {
    route.params.subscribe(params => {
      this.networkID = params['networkID'];
      this.structureID = params['structureID'];
      //toastInfo('params:  ', params);
    });
  }

  ngOnInit() {
    // reload the tabs of materializz
    $('ul.tabs').tabs();
  }

}
