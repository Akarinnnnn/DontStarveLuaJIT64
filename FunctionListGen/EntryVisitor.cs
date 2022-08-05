namespace FunctionListGen;

public record ExportItem(int Ordinal, int hint, string? Name);

public interface IEntryVisitor
{
	void Visit(ExportItem item);
}

