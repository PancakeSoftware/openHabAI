import {Component, OnDestroy, OnInit, ViewChild} from '@angular/core';
import {Api} from "@catflow/Api";
import {toastInfo, toastOk} from "../../util/Log";
import {FormBuilder, FormGroup} from "@angular/forms";
import {ApiObject} from "@catflow/ApiObject";
import { Chart, ChartData, Point } from 'chart.js';
import {AppState} from "../../projects-and-settings/settings/app-state.service";
import {componentDestroyed} from "ng2-rx-componentdestroyed";


@Component({
  selector: 'app-model-train',
  templateUrl: './model-train.component.html',
  styles: []
})
export class ModelTrainComponent implements OnInit, OnDestroy {

  public componentName = "ModelTrainComponent";

  projectID: number = -1;
  modelID: number = -1;
  trainParams: FormGroup;
  network: ApiObject<any>;

  @ViewChild('modelEditor') modelEditor;
  @ViewChild('dataChart') dataChart;
  @ViewChild('networkChart') networkChart;
  @ViewChild('networkProgressChart') networkProgressChart;
  lastProgressValue: number = -1;
  private dataChartInited: boolean = false;

  constructor(
    private state: AppState,
    private fb: FormBuilder,
    public api: Api)
  {


    // get route changes
    this.state.onProjectOrModelChange.takeUntil(componentDestroyed(this)).subscribe(route => {
      // setup form
      this.trainParams = fb.group({
          optimizer: "sgd",
          learnRate: 12,
          weightDecay: 0.1,
          initUniformScale: 0.1
        }
      );

      if (this.projectID !== route.projectId || this.modelID !== route.modelId) {
        this.projectID = route.projectId;
        this.modelID = route.modelId;
        this.updateSelf();
      }
    });
  }

  setupCharts()
  {
    //this.chartOutputShape = new ApiChart(this.api.object(`/dataStructures/${this.projectID}/dataChart/`), document.getElementById('chart-outputShape'));
  }


  ngOnInit() {
    this.setupCharts();

    if (!this.dataChartInited) {
      this.dataChart.apiObject = this.api.object('/dataStructures/' + this.projectID + '/dataChart/');
      this.networkChart.apiObject = this.api.object('/dataStructures/' + this.projectID + '/networks/' + this.modelID + '/charts/netOutput/');
      this.networkProgressChart.apiObject = this.api.object('/dataStructures/' + this.projectID + '/networks/' + this.modelID + '/charts/progress/');
      this.networkProgressChart.lastValues.subscribe(value => this.lastProgressValue = Math.round(value[0] *100000) /100000);
      //this.modelEditor.networkObject = this.network;
      this.dataChartInited = true;
    }

  }

  private updateSelf() {
    if (this.projectID == undefined || this.modelID == undefined)
      return;

    toastInfo("update ... " + this.projectID + "/" + this.modelID);

    // store self
    this.network = this.api.object(`/dataStructures/${this.projectID}/networks/${this.modelID}/`);
    // on change
    this.network.object().subscribe(net => {
      if (net == null)
        return;

      this.trainParams.setValue({
        optimizer: net.optimizer,
        learnRate: net.learnRate,
        weightDecay: net.weightDecay,
        initUniformScale: net.initUniformScale
      });
    });


    if (this.dataChart != undefined) {
      this.dataChart.apiObject = this.api.object('/dataStructures/' + this.projectID + '/dataChart/');
      this.dataChartInited = true;
    }
  }


  onTriggerTrain(startTrain: boolean) {
    if (startTrain) {
      this.network.update(this.trainParams.value).then(() =>
        this.network.action('startTrain')
          .then(() => {
            this.state.training = true;
            toastOk('start train');
          })
      );
    }
    else {
      this.network.action('stopTrain')
        .then(() => {
          this.state.training = false;
          toastOk('stopped train');
        });
    }
  }


  onTriggerResetModel() {
    this.network.update(this.trainParams.value).then(() =>
      this.network.action("resetModel")
        .then(value => toastOk("all trained data overwritten"))
    );
  }

  ngOnDestroy(): void {
  }
}
