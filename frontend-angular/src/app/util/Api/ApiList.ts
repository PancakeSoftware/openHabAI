import {Observable} from "rxjs/Observable";
import {Injectable} from "@angular/core";
import {ApiRoute, ApiRouteContainig} from "./Utils";
import {BehaviorSubject} from "rxjs/BehaviorSubject";
import {ApiConnection} from "./ApiConnection";
import {reject} from "q";

/*
 * - list
 *    > subscribe to changes of list itself(add, remove) or list and members(add, update, remove)
 *    > perform action (add, get, update, remove)
 */
@Injectable()
export class ApiList<T> extends ApiRouteContainig
{
  private onChangeItems = new BehaviorSubject<T[]>([]);

  constructor(route: ApiRoute) {
    super(route);

    // get object
    ApiConnection.sendRequest(route, 'getAll', (status, data) => {
      if (status === 'ok')
        this.onChangeItems.next(data);
    });
  }

  /**
   * get the actual list of all items
   * @returns {Observable<T[]>} update on list change (add, remove, update of object in list)
   */
  items(): Observable<T[]> {
    return  new Observable<T[]>(observable => {
      this.onChangeItems.subscribe(value => observable.next(value));
    });
  }

  /**
   * fires on changes
   * - item added:    type='add'
   * - item removed:  type='remove'
   * - item updated:  type='update'
   * @returns {Observable<{type: string; item?: T}>}
   */
  onChange(): Observable<{type: string, item?: T}> {
    return new Observable<{type: string, item?: T}>();
  }

  /**
   * adds item to list
   * @param item
   * @returns {Promise} add done
   */
  add(item: T): Promise<void>
  {
    return new Promise<void>((resolve, reject ) => {
      ApiConnection.sendRequest(this.route, "add", (status, data) => {
        if (status == 'ok')
          resolve();
        else if (status == 'error')
          reject(data.message);
      }, item);
    });
  }

  /**
   * updates object with id=id
   * @param {number} id
   * @param {T} item values to update
   * @returns {Promise} update done
   */
  update(id: number, item: T): Promise<void> {
    return new Promise<void>((resolve, reject ) => {
      // test
      setTimeout(() => {
        reject('not implemented');
      }, 1000);
    });
  }

  /**
   * deletes old item at id and creates new at id
   * this means the item object is deleted and also all its children
   * @param {number} id
   * @param {T} item
   * @returns {Promise<void>}
   */
  replace(id: number, item: T): Promise<void> {
    return new Promise<void>((resolve, reject ) => {
      reject('replace of list is not implemented yes');
    });
  }

  /**
   * removes item at id
   * @param {number} id
   * @returns {Promise<void>}
   */
  remove(id: number): Promise<void> {
    return new Promise<void>(((resolve, reject) => {

    }));
  }
}