import {ApiObject} from "../ApiObject";
import { Chart, ChartData, Point } from 'chart.js';
import {Component, EventEmitter, Input, OnDestroy, OnInit, ViewChild} from '@angular/core';
import {Subject} from "rxjs/Subject";
import "rxjs/add/operator/takeUntil";
import {componentDestroyed} from "ng2-rx-componentdestroyed";
import "rxjs/add/operator/delayWhen";
import {BehaviorSubject} from "rxjs/BehaviorSubject";
import {ReplaySubject} from "rxjs/ReplaySubject";

@Component({
  selector: 'chart-series',
  templateUrl: './chart-series.component.html',
  styles: []
})
export class ApiSeriesChart implements OnInit, OnDestroy
{
  @ViewChild("chartElement") chartElement;  // chart canvas
  public chart: Chart = null;               // chart.js object
  private chartDirty: boolean = true;
  private chartGotFirstData: boolean = false;

  /* store input values
   * index = id  */
  protected inputValues: any = {};

  private object: ApiObject<any> = null;

  public lastValues: ReplaySubject<number[]> = new ReplaySubject<number[]>();
  private lastValuesArr: number[];

  // subscriptions
  private objectUnsubscribe: Subject<void> = new Subject<void>();

  constructor() {
  }


  /**
   * set appObject object for communicating with the chart object in the backend
   * @param {ApiObject<any>} obj
   */
  @Input()  public set apiObject(obj: ApiObject<any>) {
    // cleanup old object
    this.objectUnsubscribe.next();
    this.objectUnsubscribe.complete();
    if (this.object != null)
      this.object.unsubscribe();

    // set object
    if (obj == undefined) {
      console.error("can't set undefined object for chart, you have to set object prop");
      return;
    }
    this.object = obj;

    // update self
    if (this.chart != null && this.chartDirty)
      this.refreshObject();
    else if (!this.chartDirty)
      this.chartDirty = true; // set object later after this.chart is initialised

    console.info("set obj", obj);
  }

  public get apiObject(): ApiObject<any> {
    return this.object;
  }



  private refreshObject() {
    console.info('\n\n refreshObject -------- ' + this.object.route);

    // get initial data
    this.object.action("getAllData").then(data => {
      this.lastValuesArr = [];
      // update each data-output
      for (let dataSet of data) {
        this.chart.data.datasets[dataSet[0]].data = dataSet[1].reduce((dataPoints, point) => dataPoints.concat({
          x: this.round( point.inputs[0] ),
          y: this.round( point.value )
        }), []);
        this.lastValuesArr[dataSet[0]] = dataSet[1][dataSet[1].length -1].value;
      }
      // update last values
      this.lastValues.next(this.lastValuesArr);
      this.chart.update();
    });

    // on action
    this.object.onAction()
      .takeUntil(componentDestroyed(this))
      .subscribe(data => {
        // new point
        if (data.action == 'addDataPoint') {
          //console.info("addDataPoint", data.data);
          this.chart.data.datasets[data.data.outputId].data.push({
            x: this.round( data.data.dataPoint.inputs[0] ),
            y: this.round( data.data.dataPoint.value )
          });
          // update last values
          this.lastValuesArr[data.data.outputId] = data.data.dataPoint.value;
          this.lastValues.next(this.lastValuesArr);

          this.chart.update();
        }
        // clear
        if (data.action == 'clearData') {
          for (let dataset of this.chart.data.datasets)
            dataset.data = [];

          // update last values
          for (let i in this.lastValuesArr)
            this.lastValuesArr[i] = -1;
          this.lastValues.next(this.lastValuesArr);

          this.chart.update();
        }
      });

    // set inputs/outputs when object props change
    this.object.objectChanged()
      .takeUntil(componentDestroyed(this))
      .subscribe((changed) => {
        if (changed == null)
          return;

        let obj = changed.object;

        // update input/output names
        if (changed.changedKeys.includes('inputNames') || changed.changedKeys.includes('outputNames')) {

          let color = ['#839a1a', '#308366', '#832d77'];

          // generate outputs
          this.chart.data.datasets = [];
          for (let output of obj.outputNames) {
            this.chart.data.datasets.push({
              data: [],
              label: output[1],
              backgroundColor: color[output[0]],
              borderColor: color[output[0]],
              fill: false,
              pointRadius: 0
            });
          }
          this.chart.update();
        }

        // request initial data
        if (!this.chartGotFirstData) {
          this.chartGotFirstData = true;
        }
      });

    // get data updates
    this.object.subscribe();
  }


  /**
   * setup gui stuff: link chart.js to html canvas
   */
  ngOnInit(): void {
    // setup chart.js chart
    this.chart = new Chart(( <HTMLCanvasElement> this.chartElement.nativeElement).getContext("2d"), {
      type: 'line',
      data: {
        datasets: [ /*{
          data: [],
          label: "Network",
          backgroundColor: "#3e95cd",
          borderColor: "#3e95cd",});
  charts.setSubRoutes({
          fill: false
        }, {
          data: [],
          label: "Real",
          backgroundColor: "#97e400",
          borderColor: "#97e400",
          fill: false
        } */
        ]
      },
      options: {
        legend: {
          display: true,
          position: 'right',
          labels: {
            usePointStyle: false
          }

        },
        scales: {
          xAxes: [{
            type: 'linear',
            position: 'bottom'
          }],
          yAxes: [{
            type: 'logarithmic',
            position: 'left'
          }]
        }
      }
    });

    // if need update
    if (this.chartDirty)
      this.refreshObject();
  }

  /**
   * cleanup
   */
  ngOnDestroy(): void {
    this.chart.destroy();
    this.object.unsubscribe();
    this.objectUnsubscribe.next();
    this.objectUnsubscribe.complete();
  }


  round(num: number): number {
    return Math.round(num * 100000) / 100000
  }
}