import { HttpClient, HttpClientModule } from '@angular/common/http';
import { NgModule } from '@angular/core';
import { HttpModule } from '@angular/http';
import { BrowserModule } from '@angular/platform-browser';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { RouterModule } from '@angular/router';
import { TranslateLoader, TranslateModule } from '@ngx-translate/core';
import { TranslateHttpLoader } from '@ngx-translate/http-loader';
import { AppComponent } from './app.component';
import { rootRouterConfig } from './app.routes';
import { SharedModule } from "./shared/shared.module";
import { IntroComponent } from './home/intro/intro.component';
import { HomeComponent } from './home/home.component';
import { AboutComponent } from './home/about/about.component';
import { FeatureComponent } from './home/feature/feature.component';
import { ScreenshotComponent } from './home/screenshot/screenshot.component';
import { ContactUsComponent } from './home/contact-us/contact-us.component';
import { CommonModule } from '@angular/common';
import { ReactiveFormsModule, FormsModule } from '@angular/forms';
import { OwlModule } from 'ngx-owl-carousel';
import { DownloadComponent } from './home/download/download.component';

export function createTranslateLoader(http: HttpClient) {
  return new TranslateHttpLoader(http, './assets/i18n/', '.json');
}

@NgModule({
  declarations: [
    AppComponent,
    IntroComponent,
    HomeComponent,
    IntroComponent,
    AboutComponent,
    FeatureComponent,
    ScreenshotComponent,
    ContactUsComponent,
    DownloadComponent
  ],
  imports: [
    BrowserModule.withServerTransition({ appId: 'serverApp' }),
    HttpModule,
    BrowserAnimationsModule,
    SharedModule,
    RouterModule.forRoot(rootRouterConfig, { useHash: false, anchorScrolling: 'enabled', scrollPositionRestoration: 'enabled' }),
    HttpClientModule,
    TranslateModule.forRoot({
      loader: {
        provide: TranslateLoader,
        useFactory: (createTranslateLoader),
        deps: [HttpClient]
      }
    }),
    CommonModule,
    SharedModule,
    ReactiveFormsModule,
    FormsModule,
    OwlModule
  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }

