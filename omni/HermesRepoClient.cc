namespace CAE {
void HermesRepoClient::Upload(const RepoContext &ctx, const std::string& local_path) {
    std::cout << "HermesRepoClient::Upload called for " << local_path << " to " << ctx.key_ << std::endl;
    // Real Hermes upload logic would go here
}
} // namespace CAE 