import {Component, OnInit, ViewChild} from '@angular/core';
import {Api} from "@catflow/Api";
import {ActivatedRoute} from "@angular/router";
import {toastInfo, toastOk} from "../util/Log";
import {FormBuilder, FormGroup} from "@angular/forms";
import {ApiObject} from "@catflow/ApiObject";
import { Chart, ChartData, Point } from 'chart.js';


@Component({
  selector: 'app-network-train',
  templateUrl: './network-train.component.html',
  styles: []
})
export class NetworkTrainComponent implements OnInit {

  structureID: number = -1;
  networkID: number = -1;
  trainParams: FormGroup;
  private network: ApiObject<any>;

  @ViewChild('dataChart') dataChart;
  @ViewChild('networkChart') networkChart;
  @ViewChild('networkProgressChart') networkProgressChart;
  lastProgressValue: number = -1;
  private dataChartInited: boolean = false;

  constructor(private router: ActivatedRoute,
              private fb: FormBuilder,
              public api: Api)
  {


    // get route changes
    router.params.subscribe(params => {
      // setup form
      this.trainParams = fb.group({
          optimizer: "sgd",
          learnRate: 12,
          weightDecay: 0.1
        }
      );

      if (this.structureID !== params.structureID || this.networkID !== params.networkID) {
        this.structureID = params.structureID;
        this.networkID = params.networkID;
        this.updateSelf();
      }
    });
  }

  setupCharts()
  {
    //this.chartOutputShape = new ApiChart(this.api.object(`/dataStructures/${this.structureID}/dataChart/`), document.getElementById('chart-outputShape'));


  }


  ngOnInit() {
    this.setupCharts();

    if (!this.dataChartInited) {
      this.dataChart.apiObject = this.api.object('/dataStructures/' + this.structureID + '/dataChart/');
      this.networkChart.apiObject = this.api.object('/dataStructures/' + this.structureID + '/networks/' + this.networkID + '/charts/netOutput/');
      this.networkProgressChart.apiObject = this.api.object('/dataStructures/' + this.structureID + '/networks/' + this.networkID + '/charts/progress/');
      this.networkProgressChart.lastValues.subscribe(value => this.lastProgressValue = Math.round(value[0] *100000) /100000);
      this.dataChartInited = true;
    }

  }

  private updateSelf() {
    if (this.structureID == undefined || this.networkID == undefined)
      return;

    toastInfo("update ... " + this.structureID + "/" + this.networkID);

    // store self
    this.network = this.api.object(`/dataStructures/${this.structureID}/networks/${this.networkID}/`);
    // on change
    this.network.object().subscribe(net => {
      if (net == null)
        return;

      this.trainParams.setValue({
        optimizer: net.optimizer,
        learnRate: net.learnRate,
        weightDecay: net.weightDecay
      });
    });


    if (this.dataChart != undefined) {
      this.dataChart.apiObject = this.api.object('/dataStructures/' + this.structureID + '/dataChart/');
      this.dataChartInited = true;
    }

    // on network change
    /*
    this.network.object().subscribe(value => {
      this.trainParams.reset({
        optimizer: value.optimizer,
        learnRate: value.learnRate
      });
    });
    */

    /*
    // get datastructure Chart
    let dataStructureOuts = this.api.object(`/dataStructures/${this.structureID}/dataChart/`);

    // on new data
    dataStructureOuts.onAction().subscribe(value => {
      //if (value.action == 'update')
        toastInfo('update chart: ', value);
    });

    // set params
    dataStructureOuts.update({
      fixedInputs: [
      ],
      rangeInputs: [
        {id: 0, from: 10, to: 30, steps: 2},
      ],
      outputs: [0]
    });
    toastInfo("data", dataStructureOuts.object().getValue());
    */

  }

  a: number = 11;

  onTriggerTrain(startTrain: boolean) {
    if (startTrain) {
      //toastInfo("start train: ", this.trainParams.value);
      this.network.update(this.trainParams.value);
      this.network.action('startTrain')
        .then(() => toastOk('start train'));



    }
    else {
      this.network.action('stopTrain').then(() => toastOk('stopped train'));
    }
  }


  onTriggerResetModel() {
    this.network.action("resetModel").then(value => toastOk("all trained data overwritten"))
  }
}
