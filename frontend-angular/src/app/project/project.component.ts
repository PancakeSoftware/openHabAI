import {Component, EventEmitter, OnDestroy, OnInit} from '@angular/core';
import {ActivatedRoute, Params} from "@angular/router";
import {AppState} from "@frontend/projects-and-settings/settings/app-state.service";
import {componentDestroyed} from "ng2-rx-componentdestroyed";

@Component({
  selector: 'app-project',
  templateUrl: './project.component.html',
  styles: []
})
export class ProjectComponent implements OnInit, OnDestroy {

  public componentName = "ProjectComponent";
  tabsElement: any;

  constructor(
    public state: AppState
  ) {
    // on route changes
    this.state.onProjectOrModelChange
      .takeUntil(componentDestroyed(this))
      .subscribe(route => {
        // show train tab
        if (route.modelId != undefined && (this.tabsElement != undefined)) {
          console.info('select tab', this.tabsElement);
          this.tabsElement.tabs('select_tab', 'tab-edit');
        }
      });
  }

  ngOnInit() {
    // reload the tabs of materializz
    $('ul.tabs').tabs();
    this.tabsElement = $('.nav-network .tabs');

    // show train tab
    if (this.state.routedModelId != undefined) {
      console.info('select tab on start', this.tabsElement);
      //this.tabsElement.tabs('select_tab', 'tab-edit');
    }
  }

  ngOnDestroy(): void {
  }

}
