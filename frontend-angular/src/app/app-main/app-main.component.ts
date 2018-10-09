import {Component, Inject, isDevMode, NgModule} from '@angular/core';
import {ActivatedRoute, Router, RouterModule, Routes} from "@angular/router";
import {ProjectsAndSettingsComponent} from "@frontend/projects-and-settings/projects-and-settings.component";
import {ApiConnection, CONNECTION_STATUS} from "@catflow/ApiConnection";
import {ProjectComponent} from "../project/project.component";
import {toastErr, toastInfo} from "../util/Log";
import {DOCUMENT} from "@angular/common";
import {BehaviorSubject} from "rxjs/BehaviorSubject";
import {TestPlaygroundComponent} from "@frontend/test-playground/test-playground.component";
import {AppState} from "@frontend/projects-and-settings/settings/app-state.service";

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
  alreadyTriedToConnect: boolean = false;

  // test list
  listTest: BehaviorSubject<any[]>;
  connected: boolean = false;
  port: number = 80;

  constructor(
    public state: AppState,
    public route: ActivatedRoute,
    @Inject(DOCUMENT) private document: Document)
  {
    ApiConnection.onGetError.subscribe(msg => {
      toastErr("<div><h6 class='toastRoute'>from:  " + msg.route+"</h6><p> "+ msg.message+ "</p></div>");
    });

    if (Number(this.document.location.port) == 0)
      this.port = 80;
    else if (isDevMode())
      this.port = 5555;
    else
      this.port = Number(this.document.location.port);
    ApiConnection.connect(this.document.location.hostname, this.port);
    ApiConnection.connectionStatus.subscribe(status => this.connected = (status == CONNECTION_STATUS.CONNECTED));
    ApiConnection.onDisconnect = () => {
      setTimeout(() => {
        ApiConnection.connect(this.document.location.hostname, this.port);
      }, 1000);
    }


  }

  onRouteChange($event) {
    console.log('show ', $event);
    this.routedComponent = $event.componentName;
  }
}


/*
 *-- Routing ------------------------------------------------------- */
const routes: Routes = [
  {path: '', component: ProjectsAndSettingsComponent},
  {path: 'testPlayground', component: TestPlaygroundComponent},
  {path: 'test', component: TestPlaygroundComponent},
  {path: 'project/:projectID', component: ProjectComponent},
  {path: 'project/:projectID/model/:modelID', component: ProjectComponent},
  {path: 'project/:projectID/model/:modelID/train', component: ProjectComponent}
];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule { }