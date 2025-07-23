import sys
import os
import pytest
import logging
from common import common_values
import infinity
import infinity.index as index
from infinity.common import ConflictType, InfinityException
from infinity.errors import ErrorCode

current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(current_dir)
if parent_dir not in sys.path:
    sys.path.insert(0, parent_dir)
from infinity_http import infinity_http


@pytest.fixture(scope="class")
def http(request):
    return request.config.getoption("--http")


@pytest.fixture(scope="class")
def setup_class(request, http):
    if http:
        uri = common_values.TEST_LOCAL_HOST
        request.cls.infinity_obj = infinity_http()
    else:
        uri = common_values.TEST_LOCAL_HOST
        request.cls.infinity_obj = infinity.connect(uri)
    request.cls.uri = uri
    yield
    request.cls.infinity_obj.disconnect()


@pytest.mark.usefixtures("setup_class")
@pytest.mark.usefixtures("suffix")
class TestRankFeatures:
    def test_rank_features_basic(self, suffix):
        """
        Test basic rank_features functionality
        """
        db_obj = self.infinity_obj.get_database("default_db")
        table_name = "test_rank_features_basic" + suffix

        # Drop table if exists
        res = db_obj.drop_table(table_name, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK

        # Create table
        table_obj = db_obj.create_table(
            table_name,
            {
                "id": {"type": "varchar"},
                "content": {"type": "varchar"},
                "tag": {"type": "varchar"}
            },
            ConflictType.Error
        )
        assert table_obj is not None

        # Insert test data
        res = table_obj.insert([
            {"id": "2024", "content": "hello world", "tag": '[{"Tag1":1},{"Tag2":2}]'},
            {"id": "2025", "content": "hello world", "tag": '[{"Tag1":2},{"Tag2":1}]'},
            {"id": "2026", "content": "hello world", "tag": '[{"Tag2":3},{"Tag3":4}]'}
        ])
        assert res.error_code == ErrorCode.OK

        # Create fulltext index on content
        res = table_obj.create_index(
            "content_index",
            index.IndexInfo("content", index.IndexType.FullText),
            ConflictType.Error
        )
        assert res.error_code == ErrorCode.OK

        # Create rank_features index on tag
        res = table_obj.create_index(
            "tag_index",
            index.IndexInfo("tag", index.IndexType.FullText, {"analyzer": "rankfeatures"}),
            ConflictType.Error
        )
        assert res.error_code == ErrorCode.OK

        # First test: basic fulltext search without rank_features
        res_basic, extra_result = table_obj.output(["id", "_score"]).match_text(
            "content",
            "hello world",
            10
        ).to_df()

        assert res_basic is not None
        assert len(res_basic) == 3

        # Log basic search results
        logging.info("=== Basic fulltext search results (without rank_features) ===")
        for i, row in res_basic.iterrows():
            logging.info(f"ID: {row['id']}, SCORE: {row['SCORE']}")
        logging.info("============================================================")

        # Verify basic search returns all documents with same scores (since content is identical)
        basic_scores = res_basic['SCORE'].tolist()
        assert all(score > 0 for score in basic_scores), "All basic scores should be positive"
        # All documents have same content, so scores should be identical
        assert len(set(basic_scores)) == 1, "All documents should have same score for identical content"

        # Second test: fulltext search with rank_features
        res_rank, extra_result = table_obj.output(["id", "_score"]).match_text(
            "content",
            "hello world",
            10,
            {"rank_features": "tag^Tag1^1.0,tag^Tag2^2.0"}
        ).to_df()

        assert res_rank is not None
        assert len(res_rank) == 3

        # Log rank_features search results
        logging.info("=== Fulltext search with rank_features results ===")
        for i, row in res_rank.iterrows():
            logging.info(f"ID: {row['id']}, SCORE: {row['SCORE']}")
        logging.info("=================================================")

        # Sort results by ID for consistent comparison
        res_rank_sorted = res_rank.sort_values('id').reset_index(drop=True)

        # Verify exact results match expected values from SLT test
        expected_results = [
            {"id": "2024", "score": 5.267063},
            {"id": "2025", "score": 4.267063},
            {"id": "2026", "score": 6.267063}
        ]

        assert len(res_rank_sorted) == len(expected_results)

        for i, expected in enumerate(expected_results):
            actual_id = res_rank_sorted.iloc[i]['id']
            actual_score = res_rank_sorted.iloc[i]['SCORE']

            assert actual_id == expected["id"], f"Expected ID {expected['id']}, got {actual_id}"
            assert abs(actual_score - expected["score"]) < 0.001, \
                f"Expected score {expected['score']} for ID {expected['id']}, got {actual_score}"

        # Verify that rank_features changed the scores compared to basic search
        rank_scores = res_rank['SCORE'].tolist()
        assert basic_scores != rank_scores, "Rank features should change the scores compared to basic search"

        # Clean up
        res = table_obj.drop_index("content_index", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = table_obj.drop_index("tag_index", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(table_name, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_rank_features_different_weights(self, suffix):
        """
        Test rank_features with different feature weights
        """
        db_obj = self.infinity_obj.get_database("default_db")
        table_name = "test_rank_features_weights" + suffix

        # Drop table if exists
        res = db_obj.drop_table(table_name, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK

        # Create table
        table_obj = db_obj.create_table(
            table_name,
            {
                "id": {"type": "varchar"},
                "content": {"type": "varchar"},
                "features": {"type": "varchar"}
            },
            ConflictType.Error
        )
        assert table_obj is not None

        # Insert test data with different feature values
        res = table_obj.insert([
            {"id": "doc1", "content": "test document", "features": '[{"feature1":0.8},{"feature2":0.2}]'},
            {"id": "doc2", "content": "test document", "features": '[{"feature1":0.3},{"feature2":0.7}]'},
            {"id": "doc3", "content": "test document", "features": '[{"feature1":0.5},{"feature3":0.9}]'}
        ])
        assert res.error_code == ErrorCode.OK

        # Create indexes
        res = table_obj.create_index(
            "content_idx",
            index.IndexInfo("content", index.IndexType.FullText),
            ConflictType.Error
        )
        assert res.error_code == ErrorCode.OK

        res = table_obj.create_index(
            "features_idx",
            index.IndexInfo("features", index.IndexType.FullText, {"analyzer": "rankfeatures"}),
            ConflictType.Error
        )
        assert res.error_code == ErrorCode.OK

        # First test: basic fulltext search without rank_features
        res_basic, extra_result = table_obj.output(["id", "_score"]).match_text(
            "content",
            "test document",
            10
        ).to_df()

        assert res_basic is not None
        assert len(res_basic) == 3

        # Log basic search results
        logging.info("=== Basic search results (without rank_features) ===")
        for i, row in res_basic.iterrows():
            logging.info(f"ID: {row['id']}, SCORE: {row['SCORE']}")
        logging.info("===================================================")

        # Verify basic search returns all documents with same scores (identical content)
        basic_scores = res_basic['SCORE'].tolist()
        assert all(score > 0 for score in basic_scores), "All basic scores should be positive"
        assert len(set(basic_scores)) == 1, "All documents should have same score for identical content"

        # Second test: search with feature1 boost
        res1, extra_result = table_obj.output(["id", "_score"]).match_text(
            "content",
            "test document",
            10,
            {"rank_features": "features^feature1^2.0"}
        ).to_df()

        assert res1 is not None
        assert len(res1) == 3

        # Log the results for feature1 boost
        logging.info("=== Search with feature1 boost results ===")
        for i, row in res1.iterrows():
            logging.info(f"ID: {row['id']}, SCORE: {row['SCORE']}")
        logging.info("=========================================")

        # Verify that feature1 boost affects scores differently based on feature1 values
        res1_sorted = res1.sort_values('id').reset_index(drop=True)
        scores1 = res1_sorted['SCORE'].tolist()
        assert scores1 != basic_scores, "Feature1 boost should change scores from basic search"

        # doc1 has highest feature1 value (0.8), so should have highest score
        # doc3 has middle feature1 value (0.5), doc2 has lowest (0.3)
        doc1_score = res1_sorted[res1_sorted['id'] == 'doc1']['SCORE'].iloc[0]
        doc2_score = res1_sorted[res1_sorted['id'] == 'doc2']['SCORE'].iloc[0]
        doc3_score = res1_sorted[res1_sorted['id'] == 'doc3']['SCORE'].iloc[0]

        assert doc1_score > doc3_score > doc2_score, \
            f"Expected doc1 > doc3 > doc2 based on feature1 values, got {doc1_score}, {doc3_score}, {doc2_score}"

        # Third test: search with multiple features and different boosts
        res2, extra_result = table_obj.output(["id", "_score"]).match_text(
            "content",
            "test document",
            10,
            {"rank_features": "features^feature1^1.0,features^feature2^3.0"}
        ).to_df()

        assert res2 is not None
        assert len(res2) == 3

        # Log the results for multiple features
        logging.info("=== Search with multiple features (feature1^1.0,feature2^3.0) results ===")
        for i, row in res2.iterrows():
            logging.info(f"ID: {row['id']}, SCORE: {row['SCORE']}")
        logging.info("====================================================================")

        # Verify that different feature combinations produce different scores
        res2_sorted = res2.sort_values('id').reset_index(drop=True)
        scores2 = res2_sorted['SCORE'].tolist()

        # Should be different from both basic and feature1-only searches
        assert scores2 != basic_scores, "Multiple features should change scores from basic search"
        assert scores2 != scores1, "Different feature weights should produce different scores"

        # With feature2 having 3x boost and doc2 having highest feature2 value (0.7),
        # doc2 should now have higher score than in feature1-only test
        doc1_score_multi = res2_sorted[res2_sorted['id'] == 'doc1']['SCORE'].iloc[0]
        doc2_score_multi = res2_sorted[res2_sorted['id'] == 'doc2']['SCORE'].iloc[0]
        doc3_score_multi = res2_sorted[res2_sorted['id'] == 'doc3']['SCORE'].iloc[0]

        # doc2 should benefit from high feature2 value (0.7) with 3x boost
        # doc1 has feature1=0.8, feature2=0.2, so score = 0.8*1.0 + 0.2*3.0 = 1.4
        # doc2 has feature1=0.3, feature2=0.7, so score = 0.3*1.0 + 0.7*3.0 = 2.4
        # doc3 has feature1=0.5, feature3=0.9 (no feature2), so score = 0.5*1.0 = 0.5
        assert doc2_score_multi > doc1_score_multi > doc3_score_multi, \
            f"Expected doc2 > doc1 > doc3 with feature2 boost, got {doc2_score_multi}, {doc1_score_multi}, {doc3_score_multi}"

        # Verify all scores are positive
        assert all(score > 0 for score in scores1), "All scores should be positive"
        assert all(score > 0 for score in scores2), "All scores should be positive"

        # Clean up
        res = table_obj.drop_index("content_idx", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = table_obj.drop_index("features_idx", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(table_name, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_rank_features_with_topn(self, suffix):
        """
        Test rank_features with topn parameter
        """
        db_obj = self.infinity_obj.get_database("default_db")
        table_name = "test_rank_features_topn" + suffix

        # Drop table if exists
        res = db_obj.drop_table(table_name, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK

        # Create table
        table_obj = db_obj.create_table(
            table_name,
            {
                "id": {"type": "varchar"},
                "content": {"type": "varchar"},
                "tags": {"type": "varchar"}
            },
            ConflictType.Error
        )
        assert table_obj is not None

        # Insert more test data
        test_data = []
        for i in range(10):
            test_data.append({
                "id": f"doc{i}",
                "content": "search query",
                "tags": f'[{{"tag{i % 3}":{0.1 + i * 0.1}}}]'
            })

        res = table_obj.insert(test_data)
        assert res.error_code == ErrorCode.OK

        # Create indexes
        res = table_obj.create_index(
            "content_idx",
            index.IndexInfo("content", index.IndexType.FullText),
            ConflictType.Error
        )
        assert res.error_code == ErrorCode.OK

        res = table_obj.create_index(
            "tags_idx",
            index.IndexInfo("tags", index.IndexType.FullText, {"analyzer": "rankfeatures"}),
            ConflictType.Error
        )
        assert res.error_code == ErrorCode.OK

        # First test: basic search without rank_features
        res_basic, extra_result = table_obj.output(["id", "_score"]).match_text(
            "content",
            "search query",
            10
        ).to_df()

        assert res_basic is not None
        assert len(res_basic) == 10  # All 10 documents should match

        # Log basic search results
        logging.info("=== Basic search results (without rank_features) ===")
        for i, row in res_basic.iterrows():
            logging.info(f"ID: {row['id']}, SCORE: {row['SCORE']}")
        logging.info("===================================================")

        # Verify basic search returns all documents with same scores (identical content)
        basic_scores = res_basic['SCORE'].tolist()
        assert all(score > 0 for score in basic_scores), "All basic scores should be positive"
        assert len(set(basic_scores)) == 1, "All documents should have same score for identical content"

        # Second test: search with rank_features and topn=3
        res3, extra_result = table_obj.output(["id", "_score"]).match_text(
            "content",
            "search query",
            3,
            {"rank_features": "tags^tag0^1.0,tags^tag1^1.0,tags^tag2^1.0"}
        ).to_df()

        assert res3 is not None
        assert len(res3) == 3

        # Log the results for topn=3
        logging.info("=== Search with rank_features topn=3 results ===")
        for i, row in res3.iterrows():
            logging.info(f"ID: {row['id']}, SCORE: {row['SCORE']}")
        logging.info("===============================================")

        # Verify rank_features changed the scores
        scores3 = res3['SCORE'].tolist()
        basic_top3_scores = sorted(basic_scores, reverse=True)[:3]
        assert scores3 != basic_top3_scores, "Rank features should change scores from basic search"

        # Third test: search with rank_features and topn=5
        res5, extra_result = table_obj.output(["id", "_score"]).match_text(
            "content",
            "search query",
            5,
            {"rank_features": "tags^tag0^1.0,tags^tag1^1.0,tags^tag2^1.0"}
        ).to_df()

        assert res5 is not None
        assert len(res5) == 5

        # Log the results for topn=5
        logging.info("=== Search with rank_features topn=5 results ===")
        for i, row in res5.iterrows():
            logging.info(f"ID: {row['id']}, SCORE: {row['SCORE']}")
        logging.info("===============================================")

        # Verify rank_features changed the scores for topn=5 as well
        scores5 = res5['SCORE'].tolist()
        basic_top5_scores = sorted(basic_scores, reverse=True)[:5]
        assert scores5 != basic_top5_scores, "Rank features should change scores from basic search"

        # Verify that topn=3 results are subset of topn=5 results (top 3 should be same)
        res3_sorted = res3.sort_values('SCORE', ascending=False).reset_index(drop=True)
        res5_sorted = res5.sort_values('SCORE', ascending=False).reset_index(drop=True)

        # Top 3 from topn=5 should match topn=3 results
        top3_from_5 = res5_sorted.head(3)

        # Compare IDs and scores
        for i in range(3):
            assert res3_sorted.iloc[i]['id'] == top3_from_5.iloc[i]['id'], \
                f"Top {i+1} result should be same for topn=3 and topn=5"
            assert abs(res3_sorted.iloc[i]['SCORE'] - top3_from_5.iloc[i]['SCORE']) < 0.001, \
                f"Top {i+1} score should be same for topn=3 and topn=5"

        # Clean up
        res = table_obj.drop_index("content_idx", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = table_obj.drop_index("tags_idx", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(table_name, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_rank_features_error_cases(self, suffix):
        """
        Test rank_features error cases
        """
        db_obj = self.infinity_obj.get_database("default_db")
        table_name = "test_rank_features_errors" + suffix

        # Drop table if exists
        res = db_obj.drop_table(table_name, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK

        # Create table
        table_obj = db_obj.create_table(
            table_name,
            {
                "id": {"type": "varchar"},
                "content": {"type": "varchar"},
                "tags": {"type": "varchar"}
            },
            ConflictType.Error
        )
        assert table_obj is not None

        # Insert test data
        res = table_obj.insert([
            {"id": "test1", "content": "test content", "tags": '[{"tag1":0.5}]'}
        ])
        assert res.error_code == ErrorCode.OK

        # Create content index only (no rank_features index)
        res = table_obj.create_index(
            "content_idx",
            index.IndexInfo("content", index.IndexType.FullText),
            ConflictType.Error
        )
        assert res.error_code == ErrorCode.OK

        # Test rank_features without proper index - should raise error
        with pytest.raises(InfinityException):
            table_obj.output(["id", "_score"]).match_text(
                "content",
                "test content",
                10,
                {"rank_features": "tags^tag1^1.0"}
            ).to_df()

        # Clean up
        res = table_obj.drop_index("content_idx", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(table_name, ConflictType.Error)
        assert res.error_code == ErrorCode.OK
