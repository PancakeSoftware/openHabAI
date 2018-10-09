import {Injectable, OnDestroy} from '@angular/core';
import {Api} from "@catflow/Api";
import {ActivatedRoute, NavigationEnd, NavigationStart, Router, RoutesRecognized} from "@angular/router";
import {PublishSubject} from "@frontend/util/PublishSubject";
import {BehaviorSubject, Subject} from "rxjs";
import {operators} from "rxjs/internal/Rx";
import {untilDestroyed} from "ngx-take-until-destroy";
import {componentDestroyed} from "ng2-rx-componentdestroyed";


@Injectable()
export class AppState implements OnDestroy{

  public settings:any = undefined;
  public training: boolean = false;

  public routedProjectId: number;
  public routedModelId: number;

  public onProjectChange = new PublishSubject<{projectId: number}>();
  public onProjectOrModelChange = new PublishSubject<{projectId: number, modelId: number}>();

  constructor(
    private api: Api,
    private router: Router,
    activatedRoute: ActivatedRoute
  )
  {
    this.onProjectOrModelChange.last;
    /* update routed
     */
    router.events.takeUntil(componentDestroyed(this))
      .subscribe(event => {
        if (!(event instanceof RoutesRecognized))
          return;
        if (event.state.root.children.length <= 0)
          return;

        let params = event.state.root.children[0].params;
        console.log('changed params: ', params);

        if (this.routedProjectId != params.projectID || this.routedModelId != params.modelID) {
          console.log('route changed ', params);
          let projectChanged = this.routedProjectId !== params.projectID;
          this.routedProjectId = params.projectID;
          this.routedModelId = params.modelID;

          if (projectChanged && params.projectID !== undefined)
            this.onProjectChange.next({projectId: this.routedProjectId});
          if (params.projectID !== undefined && params.modelID !== undefined)
          this.onProjectOrModelChange.next({projectId: this.routedProjectId, modelId: this.routedModelId});
        }
    });

    /* subscribe to settings
     */
    api.object('settings').object()
      .takeUntil(componentDestroyed(this))
      .subscribe((obj) => {
        this.settings = obj;
      });
    api.object('settings').subscribe();
  }


  ngOnDestroy(): void {
  }
}
