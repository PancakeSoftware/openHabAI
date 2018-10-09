import {Component, Input, OnDestroy, OnInit} from '@angular/core';
import {ActivatedRoute, Params} from "@angular/router";
import {Observable} from "rxjs/Observable";
import {toastInfo, toastOk} from "../../util/Log";
import {Api} from "../../../catflow/Api";
import {ApiList} from "../../../catflow/ApiList";
import {FormBuilder, FormGroup} from "@angular/forms";
import {BehaviorSubject} from "rxjs/BehaviorSubject";
import {AppState} from "@frontend/projects-and-settings/settings/app-state.service";
import {componentDestroyed} from "ng2-rx-componentdestroyed";

@Component({
  selector: 'app-models-list',
  templateUrl: './project-models.component.html',
  styles: []
})
export class ProjectModelsComponent implements OnInit, OnDestroy
{
  projectID: number = -1;
  formNew: FormGroup;

  models: Observable<Network[]>;
  modelsList: ApiList<Network>;
  project: BehaviorSubject<any>;

  constructor(
    fb: FormBuilder,
    private state: AppState,
    private api: Api)
  {
    // get route changes
    this.state.onProjectChange.takeUntil(componentDestroyed(this)).subscribe(route => {
      if (this.projectID !== route.projectId) {
        this.projectID = route.projectId;
        console.info('id', this.projectID);

        this.updateSelf();
      }
    });

    // form new
    this.formNew = fb.group({
      name: 'Model',
    });

    // init dropdown
    $('#train-optimizer').dropdown();
  }

  ngOnInit() {
  }

  /* reload all
   */
  updateSelf() {
    // get data
    this.project = this.api.object(`/dataStructures/${this.projectID}`).object();
    this.modelsList = this.api.list(`/dataStructures/${this.projectID}/networks`);
    this.models = this.modelsList.items();
  }

  removeNet(net: Network) {
    this.modelsList.remove(net.id);
  }

  createModel() {
    toastInfo('new model', this.formNew.value);
    this.modelsList.add(this.formNew.value);
    this.formNew.reset({
      name: 'NeuralNetwork',
    });
  }

  getNamesOf(item: any) {
    return item.map(el => el[1] + ' ');
  }

  getInputnameById(id: number) {
    return this.project.getValue().inputNames.filter(i => i[0] == id)[0][1];
  }

  ngOnDestroy(): void {
  }
}

type Network = {name: string, id: number};