import {Component, Inject, NgModule} from '@angular/core';
import {ActivatedRoute, Router, RouterModule, Routes} from "@angular/router";
import {DatastructureAndSettingsComponent} from "../datastructure-and-settings/datastructure-and-settings.component";
import {ApiConnection} from "../util/Api/ApiConnection";
import {NetworksAndTrainComponent} from "../networks-and-train/networks-and-train.component";
import {toastErr, toastInfo} from "../util/Log";
import {DOCUMENT} from "@angular/common";
import {Api} from "../util/Api/Api";
import {BehaviorSubject} from "rxjs/BehaviorSubject";

/*
 *-- MainComponent ------------------------------------------------------- */
@Component({
  selector: 'app-root',
  templateUrl: 'app-main.component.html',
  styles: []
})
export class AppMainComponent {
  title = 'app';

  routedComponent: string;
  networkID: number;
  structureID: number;

  // test list
  listTest: BehaviorSubject<any[]>;

  constructor(
    route: ActivatedRoute,
    @Inject(DOCUMENT) private document: Document)
  {
    ApiConnection.onGetError.subscribe(msg => {
      toastErr("<div><h6 class='toastRoute'>from:  " + msg.route+"</h6><p> "+ msg.message+ "</p></div>");
    });
    ApiConnection.connect(this.document.location.hostname, 5555);
  }

  onRouteChange($event) {
    console.log('show ', $event);
    this.routedComponent = $event.constructor.name;
    this.networkID = $event.networkID;
    this.structureID = $event.structureID;
    //console.log('show ', this.routedComponent);
  }
}


/*
 *-- Routing ------------------------------------------------------- */
const routes: Routes = [
  {path: '', component: DatastructureAndSettingsComponent},
  {path: 'dataStructure/:structureID', component: NetworksAndTrainComponent},
  {path: 'dataStructure/:structureID/network/:networkID', component: NetworksAndTrainComponent},
  {path: 'dataStructure/:structureID/network/:networkID/train', component: NetworksAndTrainComponent}
];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule { }