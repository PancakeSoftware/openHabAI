import {BrowserModule} from '@angular/platform-browser';
import {NgModule} from '@angular/core';
import 'materialize-css';
import {MaterializeModule, MaterializeDirective, MaterializeAction} from 'angular2-materialize';
import {FormGroup, FormsModule, ReactiveFormsModule} from '@angular/forms';
import { ApPrismModule } from '@angular-package/prism';
import {NgxAutoScrollModule} from "ngx-auto-scroll";


import {AppMainComponent, AppRoutingModule} from './app-main/app-main.component';
import {DatastructureAndSettingsComponent} from './datastructure-and-settings/datastructure-and-settings.component';
import {DataStructuresComponent} from './data-structures/data-structures.component';
import {BackendConsoleComponent} from "./backend-console/backend-console.component";
import { NetworksAndTrainComponent } from './networks-and-train/networks-and-train.component';
import { NetworksComponent } from './networks/networks.component';
import {RouteReuseStrategy} from "@angular/router";
import { NetworkTrainComponent } from './network-train/network-train.component';
import {Api} from "@catflow/Api";
import {ApiChart} from "@catflow/extensions/Chart-range";
import {KeysPipe} from "@catflow/Utils";
import { SettingsComponent } from './settings/settings.component';
import { SelectionViewComponent } from './settings/selection-view/selection-view.component';
import {AppState} from "@frontend/settings/app-state.service";
import {ApiSeriesChart} from "@catflow/extensions/Chart-series";
import { ModelEditorComponent } from './test-playground/model-editor/model-editor.component';
import { TestPlaygroundComponent } from './test-playground/test-playground.component';
//import { ChartPlotlyComponent } from './test-playground/chart-plotly/chart-plotly.component';
import { ModelEditorNodeComponent } from './test-playground/model-editor/model-editor-node/model-editor-node.component';
import { ModelEditorConnectionComponent } from './test-playground/model-editor/model-editor-connection/model-editor-connection.component';
import { ProgressIndicatorComponent } from './util/progress-indicator/progress-indicator.component';



@NgModule({
  declarations: [
    AppMainComponent,
    DatastructureAndSettingsComponent,
    DataStructuresComponent,
    BackendConsoleComponent,
    NetworksAndTrainComponent,
    NetworksComponent,
    NetworkTrainComponent,
    ApiChart,
    ApiSeriesChart,
    KeysPipe,
    SettingsComponent,
    SelectionViewComponent,
    ModelEditorComponent,
    TestPlaygroundComponent,
  //  ChartPlotlyComponent,
    ModelEditorNodeComponent,
    ModelEditorConnectionComponent,
    ProgressIndicatorComponent
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
    Api,
    AppState
    /*{provide: RouteReuseStrategy, useClass: CustomReuseStrategy}*/
  ],
  entryComponents: [
    ModelEditorNodeComponent,
    ModelEditorConnectionComponent
  ],
  bootstrap: [AppMainComponent]
})
export class AppModule {
}
