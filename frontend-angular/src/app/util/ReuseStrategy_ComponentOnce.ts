// This impl. bases upon one that can be found in the router's test cases.
import {ActivatedRouteSnapshot, DetachedRouteHandle, RouteReuseStrategy} from "@angular/router";

export class CustomReuseStrategy implements RouteReuseStrategy {
  /* This strategy only generates each component only once
   */

  handlers: {[key: string]: DetachedRouteHandle} = {};

  shouldDetach(route: ActivatedRouteSnapshot): boolean {
    console.debug('CustomReuseStrategy:shouldDetach', route);
    return true; // reuse all routes
  }

  store(route: ActivatedRouteSnapshot, handle: DetachedRouteHandle): void {
    console.debug('CustomReuseStrategy:store [['+ this.getKey(route) +']]', route, handle);
    //if (!!handle)
    this.handlers[this.getKey(route)] = handle;
    //this.handlers[route.routeConfig.path] = handle;
  }

  shouldAttach(route: ActivatedRouteSnapshot): boolean {
    console.debug('CustomReuseStrategy:shouldAttach [['+this.getKey(route)+']]', route);
    return !!route.routeConfig && !!this.handlers[this.getKey(route)];
    //return !!route.routeConfig && !!this.handlers[route.routeConfig.path]; // in map
  }

  retrieve(route: ActivatedRouteSnapshot): DetachedRouteHandle {
    console.debug('CustomReuseStrategy:retrieve [['+this.getKey(route)+']]', route);
    if (!route.routeConfig) return null;
    return this.handlers[this.getKey(route)];
    //return this.handlers[route.routeConfig.path];
  }

  shouldReuseRoute(future: ActivatedRouteSnapshot, curr: ActivatedRouteSnapshot): boolean {
    console.debug('CustomReuseStrategy:shouldReuseRoute', future, curr);
    return future.routeConfig === curr.routeConfig;
  }

  private getKey(route: ActivatedRouteSnapshot) {
    let key: string;
    if (route.component) {
      key = route.component['name'];
    } else {
      key = route.firstChild.component['name'];
    }
    return key;
  }

}