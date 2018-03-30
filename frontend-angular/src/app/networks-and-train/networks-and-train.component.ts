import {Component, EventEmitter, OnInit} from '@angular/core';
import {ActivatedRoute, Params} from "@angular/router";
import {toastInfo} from "../util/Log";
import {Observable} from "rxjs/Observable";

@Component({
  selector: 'app-networks-and-train',
  templateUrl: './networks-and-train.component.html',
  styles: []
})
export class NetworksAndTrainComponent implements OnInit {


  constructor(private router: ActivatedRoute) {
    router.params.subscribe(params => {
    });
  }

  ngOnInit() {
  }

}
