import { Routes, RouterModule } from '@angular/router';
import { DemoComponent } from './demo/demo.component';
import { BlogComponent } from './blog/blog.component';

export const rootRouterConfig: Routes = [
  { 
    path: '', 
    redirectTo: 'demo', 
    pathMatch: 'full' 
  },
  { 
    path: 'demo',
    component: DemoComponent
  },
  { 
    path: 'home', 
    loadChildren: () => import('./home/home.module').then(m=>m.HomeModule)
  },
  { 
    path: 'blog',
    component: BlogComponent,
    loadChildren: () => import('./blog/blog.module').then(m=>m.BlogModule)
  },
  { 
    path: 'pages',
    loadChildren: () => import('./pages/pages.module').then(m=>m.PagesModule)
  },
  { 
    path: '**', 
    redirectTo: 'home/one'
  }
];

