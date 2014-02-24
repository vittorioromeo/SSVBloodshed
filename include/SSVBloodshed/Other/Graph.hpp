// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef OB_TESTING_OTHER_GRAPH
#define OB_TESTING_OTHER_GRAPH

#include <vector>
#include <memory>
#include <SSVUtils/SSVUtils.hpp>

namespace ssvut
{
	namespace Internal
	{
		template<typename TGraph> class GraphLink : public TGraph::StorageLinkBase
		{
			private:
				using NodePtr = typename TGraph::NodePtr;
				NodePtr node{TGraph::getNodeNull()};

			public:
				inline GraphLink(const NodePtr& mNode) noexcept : node{mNode} { SSVU_ASSERT(TGraph::isNodeValid(node)); }
				inline const NodePtr& getNode() const noexcept { SSVU_ASSERT(TGraph::isNodeValid(node)); return node; }
		};

		template<typename TGraph> class GraphNode : public TGraph::StorageNodeBase
		{
			private:
				using NodePtr = typename TGraph::NodePtr;
				using NodeDerived = typename TGraph::NodeDerived;

			public:
				template<typename... TArgs> inline void linkToSelf(TArgs&&... mArgs)
				{
					TGraph::StorageNodeBase::emplaceLink(TGraph::StorageNodeBase::getNodePtr(this), std::forward<TArgs>(mArgs)...);
				}
				template<typename... TArgs> inline void linkTo(const NodePtr& mNode, TArgs&&... mArgs)
				{
					SSVU_ASSERT(TGraph::isNodeValid(mNode));
					TGraph::StorageNodeBase::emplaceLink(mNode, std::forward<TArgs>(mArgs)...);
				}

				inline const decltype(TGraph::StorageNodeBase::links)& getLinks() const	{ return TGraph::StorageNodeBase::links; }
				inline bool isIsolated() const noexcept	{ return TGraph::StorageNodeBase::links.empty(); }
		};

		template<typename TGraph> struct GraphStorageFreeStore
		{
			using NodeDerived = typename TGraph::NodeDerived;
			using LinkDerived = typename TGraph::LinkDerived;
			using NodePtr = NodeDerived*;

			struct NodeBase
			{
				std::vector<LinkDerived> links;
				template<typename... TArgs> inline void emplaceLink(TArgs&&... mArgs) { links.emplace_back(std::forward<TArgs>(mArgs)...); }
				inline static NodePtr getNodePtr(GraphNode<TGraph>* mNode) noexcept { return reinterpret_cast<NodeDerived*>(mNode); }
			};
			struct LinkBase { };

			std::vector<ssvu::Uptr<NodeDerived>> nodes;

			inline static const NodePtr& getNodeNull() noexcept { static NodePtr result{nullptr}; return result; }
			inline static constexpr bool isNodeValid(const NodePtr& mNode) noexcept { return mNode != getNodeNull(); }

			template<typename... TArgs> inline NodePtr createNode(TArgs&&... mArgs)
			{
				SSVU_ASSERT_STATIC(ssvu::isBaseOf<GraphNode<TGraph>, NodeDerived>(), "TNode must be derived from Graph::Node");
				return &ssvu::getEmplaceUptr<NodeDerived>(nodes, std::forward<TArgs>(mArgs)...);
			}
		};
	}

	template<typename TNode, typename TLink, template<typename> class TStorage = Internal::GraphStorageFreeStore> class Graph
	{
		public:
			using Node = Internal::GraphNode<Graph>;
			using Link = Internal::GraphLink<Graph>;
			using NodeDerived = TNode;
			using LinkDerived = TLink;
			using Storage = TStorage<Graph>;
			using StorageNodeBase = typename Storage::NodeBase;
			using StorageLinkBase = typename Storage::LinkBase;
			using NodePtr = typename Storage::NodePtr;
			friend Storage;
			friend Node;
			friend Link;

		private:
			Storage storage;
			std::vector<NodePtr> nodes;

		protected:
			template<typename... TArgs> inline NodePtr createNode(TArgs&&... mArgs)
			{
				auto result(storage.createNode(std::forward<TArgs>(mArgs)...));
				nodes.emplace_back(result); return result;
			}

		public:
			inline const decltype(nodes)& getNodes() const noexcept				{ return nodes; }
			inline decltype(nodes)& getNodes() noexcept							{ return nodes; }
			inline static const NodePtr& getNodeNull() noexcept					{ return Storage::getNodeNull(); }
			inline static constexpr bool isNodeValid(NodePtr mNode) noexcept	{ return Storage::isNodeValid(mNode); }
			inline const NodePtr& getLastAddedNode() noexcept					{ SSVU_ASSERT(!nodes.empty()); return nodes.back(); }
	};
}

#endif
