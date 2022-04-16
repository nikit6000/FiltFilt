//
//  ViewController.swift
//  FiltFilt
//
//  Created by nikit6000 on 04/16/2022.
//  Copyright (c) 2022 nikit6000. All rights reserved.
//

import UIKit
import FiltFilt

class ViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
       
        let hp = try? FiltFiltSwift(
            parameters: .init(
                a: [1.0, -0.999975456608585],
                b: [0.999987728304293, -0.999987728304293]
            )
        )
        
        let lp = try? FiltFiltSwift(
            parameters: .init(
                a: [1.0, -0.884239215225350],
                b: [0.0578803923873251, 0.0578803923873251]
            )
        )
        
        guard let hp = hp, let lp = lp else {
            return
        }
        
        let output = hp.process(input: RawData.data).map {
            abs($0)
        }
        
        let outputLP = lp.process(input: output)
        
        UIPasteboard.general.string = outputLP.map {
            "\($0 < 0.05 ? 1 : 0)"
        }
        .joined(separator: "\n")
    }
}

