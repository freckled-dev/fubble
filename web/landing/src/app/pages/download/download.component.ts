import { Component, OnInit } from '@angular/core';
import {Meta, Title} from '@angular/platform-browser';
import {ActivatedRoute} from '@angular/router';

@Component({
  selector: 'app-download',
  templateUrl: './download.component.html',
  styleUrls: ['./download.component.scss']
})
export class DownloadComponent implements OnInit {

  constructor(private route: ActivatedRoute,
    private title: Title,
    private meta: Meta) { }

  ngOnInit() {
    this.title.setTitle( this.route.snapshot.data['title']);
  }

}
