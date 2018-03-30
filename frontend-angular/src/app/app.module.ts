import {BrowserModule} from '@angular/platform-browser';
import {NgModule} from '@angular/core';
import 'materialize-css';
import {MaterializeModule, MaterializeDirective, MaterializeAction} from 'angular2-materialize';
import {FormGroup, FormsModule, ReactiveFormsModule} from '@angular/forms';
import { ApPrismModule } from '@angular-package/prism';
import {NgxAutoScrollModule} from "ngx-auto-scroll";
import { FormBuilder } from '@angular/forms';


import {AppMainComponent, AppRoutingModule} from './app-main/app-main.component';
import {DatastructureAndSettingsComponent} from './datastructure-and-settings/datastructure-and-settings.component';
import {DataStructuresComponent} from './data-structures/data-structures.component';
import {BackendConsoleComponent} from "./backend-console/backend-console.component";
import { NetworksAndTrainComponent } from './networks-and-train/networks-and-train.component';
import {ClickStopPropagation} from "./util/Directives";
import { NetworksComponent } from './networks/networks.component';
import {CustomReuseStrategy} from "./util/ReuseStrategy";
import {RouteReuseStrategy} from "@angular/router";
import { NetworkTrainComponent } from './network-train/network-train.component';



@NgModule({
  declarations: [
    AppMainComponent,
    DatastructureAndSettingsComponent,
    DataStructuresComponent,
    BackendConsoleComponent,
    NetworksAndTrainComponent,
    NetworksComponent,
    NetworkTrainComponent
  ],
  imports: [
    BrowserModule,
    AppRoutingModule,
    MaterializeModule,
    FormsModule,
    ApPrismModule,
    NgxAutoScrollModule,
    ReactiveFormsModule
  ],
  providers: [
    /*{provide: RouteReuseStrategy, useClass: CustomReuseStrategy}*/
  ],
  bootstrap: [AppMainComponent]
})
export class AppModule {
}
