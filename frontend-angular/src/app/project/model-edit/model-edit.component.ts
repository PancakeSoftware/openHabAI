import {Component, OnDestroy, OnInit} from '@angular/core';
import {AppState} from "@frontend/projects-and-settings/settings/app-state.service";
import {componentDestroyed} from "ng2-rx-componentdestroyed";
import {ApiObject} from "@catflow/ApiObject";
import {Api} from "@catflow/Api";

@Component({
  selector: 'app-model-edit',
  templateUrl: './model-edit.component.html',
  styles: []
})
export class ModelEditComponent implements OnInit, OnDestroy {

  model: ApiObject<any>;

  constructor(
    state: AppState,
    api: Api
  ) {
    state.onProjectOrModelChange.takeUntil(componentDestroyed(this))
      .subscribe(route => {
        console.log('onProjectOrModelChange', route);
        this.model = api.object(`/dataStructures/${route.projectId}/networks/${route.modelId}`)
    });
  }

  ngOnInit() {
  }

  ngOnDestroy(): void {
  }
}
