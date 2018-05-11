import {Observable} from "rxjs/Observable";
import {Injectable} from "@angular/core";
import {ApiRoute, ApiRouteContainig} from "./Utils";
import {ApiConnection} from "./ApiConnection";
import {Subject} from "rxjs/Subject";
import {observable} from "rxjs/symbol/observable";
import {BehaviorSubject} from "rxjs/BehaviorSubject";
import {Promise} from "q";
import {Api} from "./Api";
import {ReplaySubject} from "rxjs/ReplaySubject";

/*
 * - objects
 *    > subscribe to changes (add, update, remove)
 *    > perform action (add, get, update, remove)
 *
 * - send custom action to list, object
 *    > send [action: 'myAction', data: {...}]
 *    > get response [status: 'ok', data: {...}]
 */
@Injectable()
export class ApiObject<T> extends ApiRouteContainig
{
  protected onChange = new BehaviorSubject<T>(null);
  protected onActionReceive = new Subject<{action: string, data:any}>();
  protected subscribing: boolean = false;

  constructor(route: ApiRoute) {
    super(route);
    console.info('create Object: ', route);

    // get object
    ApiConnection.sendRequest(route, 'get', (status, data) => {
      if (status === 'ok')
        this.onChange.next(data);
    });

    // listen for actions
    ApiConnection.listenForAction(route, (action, data) => {
      if (action !== '' )
        this.onActionReceive.next({action, data});
    });
  }

  /**
   * get the actual object
   * @returns {Observable<T[]>} update on object change
   * @see subscribe()
   */
  object(): BehaviorSubject<T> {
    return this.onChange;
  }

  /**
   * on action is received
   * @returns BehaviorSubject<{action: string; data: any}>
   * @see subscribe()
   */
  onAction(): Subject<{action: string, data:any}> {
    return this.onActionReceive;
  }

  /**
   * updates params of object
   * @param params
   * @returns {Promise} update done
   */
  update(params: any): Promise<void> {
    return Promise<void>(((resolve, reject) => {
      ApiConnection.sendRequest(this.route, 'update', (status, data) => {
        if (status == 'ok')
          resolve(data);
        else
          reject(data);
      }, params);
    }));
  }

  /**
   * send action to object
   * @param {string} action
   * @param data
   * @returns {Promise} respond
   */
  action(action: string, data?: any): Promise<any> {
    return Promise<any>(((resolve, reject) => {
      ApiConnection.sendRequest(this.route, action, (status, data) => {
        if (status == 'ok')
          resolve(data);
        else
          reject(data);
      }, data);
    }));
  }

  /**
   * subscribe to changes of object
   * this sends subscribe action to backend
   */
  subscribe(): void {
    // subscribe to changes
    if (!this.subscribing) {
      ApiConnection.sendRequest(this.route, 'subscribe');
      this.subscribing = true;
    }
  }

  /**
   * unsubscribe to changes of object
   * this sends unsubscribe action to backend
   */
  unsubscribe(): void {
    // unsubscribe to changes
    if (this.subscribing) {
      ApiConnection.sendRequest(this.route, 'unsubscribe');
      this.subscribing = false;
    }
  }


}