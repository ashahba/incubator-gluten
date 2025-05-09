/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package org.apache.spark.sql

import org.apache.gluten.exception.GlutenException

import org.apache.spark.{SparkContext, TestUtils}
import org.apache.spark.scheduler.SparkListener
import org.apache.spark.sql.test.SQLTestUtils

object GlutenTestUtils {
  def compareAnswers(actual: Seq[Row], expected: Seq[Row], sort: Boolean = false): Unit = {
    val result = SQLTestUtils.compareAnswers(actual, expected, sort)
    if (result.isDefined) {
      throw new GlutenException("Failed to compare answer" + result.get)
    }
  }

  def withListener[L <: SparkListener](sc: SparkContext, listener: L)(body: L => Unit): Unit = {
    TestUtils.withListener(sc, listener)(body)
  }
}
