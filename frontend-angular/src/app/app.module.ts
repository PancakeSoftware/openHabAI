import {BrowserModule} from '@angular/platform-browser';
import {NgModule} from '@angular/core';
import 'materialize-css';
import {MaterializeModule, MaterializeDirective, MaterializeAction} from 'angular2-materialize';
import {FormGroup, FormsModule, ReactiveFormsModule} from '@angular/forms';
import { ApPrismModule } from '@angular-package/prism';
import {NgxAutoScrollModule} from "ngx-auto-scroll";


import {AppMainComponent, AppRoutingModule} from './app-main/app-main.component';
import {ProjectsAndSettingsComponent} from './projects-and-settings/projects-and-settings.component';
import {ProjectsListComponent} from './projects-and-settings/projects-list/projects-list.component';
import {BackendConsoleComponent} from "./backend-console/backend-console.component";
import { ProjectComponent } from './project/project.component';
import {RouteReuseStrategy} from "@angular/router";
import { ModelTrainComponent } from './project/model-train/model-train.component';
import {Api} from "@catflow/Api";
import {ApiChart} from "@catflow/extensions/Chart-range";
import {KeysPipe} from "@catflow/Utils";
import { SettingsComponent } from './projects-and-settings/settings/settings.component';
import { SelectionViewComponent } from './projects-and-settings/settings/selection-view/selection-view.component';
import {AppState} from "@frontend/projects-and-settings/settings/app-state.service";
import {ApiSeriesChart} from "@catflow/extensions/Chart-series";
import { ModelEditorComponent } from './project/model-edit/model-editor/model-editor.component';
import { TestPlaygroundComponent } from './test-playground/test-playground.component';
//import { ChartPlotlyComponent } from './test-playground/chart-plotly/chart-plotly.component';
import { ModelEditorNodeComponent } from './project/model-edit/model-editor/model-editor-node/model-editor-node.component';
import { ModelEditorConnectionComponent } from './project/model-edit/model-editor/model-editor-connection/model-editor-connection.component';
import { ProgressIndicatorComponent } from './util/progress-indicator/progress-indicator.component';
import {ProjectModelsComponent} from "@frontend/project/models-list/project-models.component";
import { ModelEditComponent } from './project/model-edit/model-edit.component';



@NgModule({
  declarations: [
    AppMainComponent,
    ProjectsAndSettingsComponent,
    ProjectsListComponent,
    BackendConsoleComponent,
    ProjectComponent,
    ProjectModelsComponent,
    ModelTrainComponent,
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
    ProgressIndicatorComponent,
    ModelEditComponent
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
