import {Component, NgModule} from '@angular/core';
import {ActivatedRoute, Router, RouterModule, Routes} from "@angular/router";
import {DatastructureAndSettingsComponent} from "../datastructure-and-settings/datastructure-and-settings.component";
import {toast} from "angular2-materialize";
import {ApiConnection} from "../util/Api/ApiConnection";
import {NetworksAndTrainComponent} from "../networks-and-train/networks-and-train.component";
import {DataStructuresComponent} from "../data-structures/data-structures.component";
import {NetworksComponent} from "../networks/networks.component";
import {NetworkTrainComponent} from "../network-train/network-train.component";
import {toastInfo} from "../util/Log";

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

  constructor(route: ActivatedRoute) {
    ApiConnection.connect();
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
  {path: 'dataStructure/:structureID/network/:networkID', component: NetworksAndTrainComponent}
];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule { }