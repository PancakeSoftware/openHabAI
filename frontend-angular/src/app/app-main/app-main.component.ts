import {Component, NgModule} from '@angular/core';
import {Router, RouterModule, Routes} from "@angular/router";
import {DatastructureAndSettingsComponent} from "../datastructure-and-settings/datastructure-and-settings.component";
import {toast} from "angular2-materialize";
import {Backend} from "../util/backendSocket";
import {NetworksAndTrainComponent} from "../networks-and-train/networks-and-train.component";
import {DataStructuresComponent} from "../data-structures/data-structures.component";
import {NetworksComponent} from "../networks/networks.component";
import {NetworkTrainComponent} from "../network-train/network-train.component";

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

  constructor(private router: Router) {
    Backend.connect();
  }

  onRouteChange() {
    console.log('show ', this.routedComponent);
  }
}


/*
 *-- Routing ------------------------------------------------------- */
const routes: Routes = [
  {path: '', component: DatastructureAndSettingsComponent},
  {path: 'dataStructure/:ID_DataStructure', component: NetworksAndTrainComponent,
    children: [
      {path: '', component: NetworksComponent},
      {path: 'network/:ID_Network', component: NetworkTrainComponent}
    ]
  },

];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule { }