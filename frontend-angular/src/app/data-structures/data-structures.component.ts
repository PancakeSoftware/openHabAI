import {Component, ElementRef, EventEmitter, OnDestroy, OnInit, ViewChild, ViewChildren} from '@angular/core';
import {MaterializeAction} from "angular2-materialize";
import {FormBuilder, FormGroup} from "@angular/forms";
import {toastErr, toastInfo, toastOk} from "../util/Log";
import {Observable} from "rxjs/Observable";
import {Tabs} from "../util/Helper";
import {Api} from "@catflow/Api";
import {ApiList} from "@catflow/ApiList";


@Component({
  selector: 'app-data-structures',
  templateUrl: './data-structures.component.html',
  styles: []
})
export class DataStructuresComponent implements OnInit, OnDestroy {

  modalActions = new EventEmitter<string|MaterializeAction>();


  dataStructuresList: ApiList<any>;
  dataStructures: Observable<any[]>;

  formNew: FormGroup;
  formNewTypeTabs: Tabs;
  formNewInputsList: {name: string, from: number, to: number, steps: number}[];


  /* setup all
   */
  constructor(
    fb: FormBuilder,
    hostElement: ElementRef,
    private api: Api)
  {
    // get data
    this.dataStructuresList = api.list<any>("/dataStructures");
    this.dataStructures = this.dataStructuresList.items();


    // control tabs
    this.formNewTypeTabs = new Tabs(hostElement, '.dStructure-type','type');

    // new datastruture form
    this.formNew = fb.group({
      name: 'MyDataStructure',
      type: '',
      function: 'x^3 + y^2',
    });
    this.formNewInputsList = [
      {name: 'x', from: -1, to: 1, steps: 10},
      {name: 'y', from: -1, to: 1, steps: 10}
      ];

    // get data
    /*
    ApiConnection.sendRequest([{"dataStructures": ""}], "getAll", (what, data) => {
      if (what == 'ok')
        this.dataStructures = data;  //data.map(item => this.dataStructures.push(item));
    });
    */
  }

  formNewInputsListAddNew(){
    this.formNewInputsList.push({name: 'x', from: -1, to: 1, steps: 10})
  }

  ngOnInit() {
  }



  /* new dataStructure
   */
  createNew() {
    // set type by tabs
    this.formNew.value.type = "function";//this.formNewTypeTabs.active;
    this.formNew.value.outputNames = ['function'];
    //toastInfo('New DataStructure: ', this.formNew.value);

    this.dataStructuresList.add({
      ...this.formNew.value,
      ...{
        inputRanges: this.formNewInputsList.map(input => {return {from: input.from, to: input.to, steps: input.steps}; }),
        inputNames: this.formNewInputsList.map(input => input.name)
      }
    }).then(value => toastOk('Added dataStructure (id: ' + value.id + '). Created  <b>' + value.dataRecords + 'data-records</b> ', 5000))
      .catch(reason => toastErr("can't add dataStructure: " + reason));

    this.formNew.reset({
      name: 'MyDataStructure',
      type: '',
      function: 'x^3 + y^2',
    });
    this.formNewInputsList = [
      {name: 'x', from: -1, to: 1, steps: 10},
      {name: 'y', from: -1, to: 1, steps: 10}
      ];
  }

  removeStruc(struc: DataStructure) {
    this.dataStructuresList.remove(struc.id);
  }

  ngOnDestroy(): void {
    console.log('dataStructures destroy!');
  }
}



type DataStructure = {name: string, id: number, type: string};