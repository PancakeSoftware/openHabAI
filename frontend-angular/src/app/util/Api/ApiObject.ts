import {Observable} from "rxjs/Observable";
import {Injectable} from "@angular/core";
import {ApiRoute, ApiRouteContainig} from "./Utils";
import {ApiConnection} from "./ApiConnection";
import {Subject} from "rxjs/Subject";
import {observable} from "rxjs/symbol/observable";
import {BehaviorSubject} from "rxjs/BehaviorSubject";

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
  private onChange = new BehaviorSubject<T>(null);

  constructor(route: ApiRoute) {
    super(route);
    console.info('create Object: ', route);

    // get object
    ApiConnection.sendRequest(route, 'get', (status, data) => {
      if (status === 'ok')
        this.onChange.next(data);
    });
  }

  /**
   * get the actual object
   * @returns {Observable<T[]>} update on object change
   */
  object(): Observable<T> {
    return  new Observable<T>(observable => {
      this.onChange.subscribe(value => observable.next(value));
    });
  }

  /**
   * updates params of object
   * @returns {Promise} update done
   */
  update(): Promise<void> {
    return new Promise<void>(((resolve, reject) => {

    }));
  }


}