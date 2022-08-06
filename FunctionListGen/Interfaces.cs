namespace FunctionListGen;

public record ExportItem(int Ordinal, int HintIndex, string? Name);

public interface IEntryVisitor
{
	void Visit(ExportItem item);
}

public interface IEmitter
{
	void Emit(string outDir);
}