import {Component, ElementRef, EventEmitter, OnDestroy, OnInit, ViewChild, ViewChildren} from '@angular/core';
import {MaterializeAction} from "angular2-materialize";
import {FormBuilder, FormGroup} from "@angular/forms";
import {toastErr, toastInfo, toastOk} from "../../util/Log";
import {Observable} from "rxjs/Observable";
import {Tabs} from "../../util/Helper";
import {Api} from "../../../catflow/Api";
import {ApiList} from "../../../catflow/ApiList";


@Component({
  selector: 'app-projects-list',
  templateUrl: './projects-list.component.html',
  styles: []
})
export class ProjectsListComponent implements OnInit, OnDestroy {

  modalActions = new EventEmitter<string|MaterializeAction>();


  modelsList: ApiList<any>;
  models: Observable<any[]>;

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
    this.modelsList = api.list<any>("/dataStructures");
    this.models = this.modelsList.items();


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

    this.modelsList.add({
      ...this.formNew.value,
      ...{
        inputRanges: this.formNewInputsList.map((input, index) => {return {from: input.from, to: input.to, steps: input.steps, id: index}; }),
        inputNames: this.formNewInputsList.map(input => input.name)
      }
    }).then(value => toastOk('Added project (id: ' + value.id + '). Created  <b>' + value.dataRecords + 'data-records</b> ', 5000))
      .catch(reason => toastErr("can't add project: " + reason));

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
    this.modelsList.remove(struc.id);
  }

  ngOnDestroy(): void {
    console.log('dataStructures destroy!');
  }
}



type DataStructure = {name: string, id: number, type: string};