import {Component, Inject, NgModule} from '@angular/core';
import {ActivatedRoute, Router, RouterModule, Routes} from "@angular/router";
import {DatastructureAndSettingsComponent} from "@frontend/datastructure-and-settings/datastructure-and-settings.component";
import {ApiConnection, CONNECTION_STATUS} from "@catflow/ApiConnection";
import {NetworksAndTrainComponent} from "../networks-and-train/networks-and-train.component";
import {toastErr, toastInfo} from "../util/Log";
import {DOCUMENT} from "@angular/common";
import {BehaviorSubject} from "rxjs/BehaviorSubject";
import {TestPlaygroundComponent} from "@frontend/test-playground/test-playground.component";
import {AppState} from "@frontend/settings/app-state.service";

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
  alreadyTriedToConnect: boolean = false;

  // test list
  listTest: BehaviorSubject<any[]>;
  connected: boolean = false;

  constructor(
    public appState: AppState,
    public route: ActivatedRoute,
    @Inject(DOCUMENT) private document: Document)
  {
    ApiConnection.onGetError.subscribe(msg => {
      toastErr("<div><h6 class='toastRoute'>from:  " + msg.route+"</h6><p> "+ msg.message+ "</p></div>");
    });

    // this.document.location.hostname
    ApiConnection.connect(this.document.location.hostname, 5555);
    ApiConnection.connectionStatus.subscribe(status => this.connected = (status == CONNECTION_STATUS.CONNECTED));
    ApiConnection.onDisconnect = () => {
      setTimeout(() => {
        ApiConnection.connect(this.document.location.hostname, 5555);
      }, 1000);
    }


  }

  onRouteChange($event) {
    console.log('show ', $event);
    this.routedComponent = $event.componentName;
    this.networkID = $event.networkID;
    this.structureID = $event.structureID;
    //console.log('show ', this.routedComponent);
  }
}


/*
 *-- Routing ------------------------------------------------------- */
const routes: Routes = [
  {path: '', component: DatastructureAndSettingsComponent},
  {path: 'testPlayground', component: TestPlaygroundComponent},
  {path: 'test', component: TestPlaygroundComponent},
  {path: 'dataStructure/:structureID', component: NetworksAndTrainComponent},
  {path: 'dataStructure/:structureID/network/:networkID', component: NetworksAndTrainComponent},
  {path: 'dataStructure/:structureID/network/:networkID/train', component: NetworksAndTrainComponent}
];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule { }