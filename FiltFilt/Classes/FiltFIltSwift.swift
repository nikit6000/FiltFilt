//
//  FiltFIltSwift.swift
//  FiltFIltSwift
//
//  Created by Nikita Tarkhov on 10.04.2022.
//

import Foundation

public final class FiltFiltSwift {
    
    // MARK: - types
    
    public enum Error: Swift.Error {
        case parametersLenghtNotMatch
    }
    
    public struct Parameters {
        public let a: [Double]
        public let b: [Double]
        
        public init(a: [Double], b: [Double]) {
            self.a = a
            self.b = b
        }
    }
    
    // MARK: - private vars
    
    private let parameters: Parameters
    private let order: Int
    
    // MARK: - initialization
    
    public init(parameters: Parameters) throws {
        guard parameters.b.count == parameters.a.count else {
            throw Error.parametersLenghtNotMatch
        }
        
        self.parameters = parameters
        self.order = parameters.a.count
    }
    
    // MARK: - Public methods
    
    public func process(input x: [Double]) -> [Double] {
        var output = [Double](repeating: .zero, count: x.count)
        
        output.withUnsafeMutableBufferPointer { mutableBufferPointer in
            filtfilt(parameters.a, parameters.b, order, x, mutableBufferPointer.baseAddress, mutableBufferPointer.count)
        }
        
        return output
    }
}
