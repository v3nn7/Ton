const vscode = require('vscode');

function activate(context) {
    console.log('Congratulations, your extension "ton-language-support" is now active!');

    const TON_KEYWORDS = [
        'let', 'const', 'fn', 'if', 'else', 'loop', 'return', 'while', 'for', 'print',
        'switch', 'case', 'default', 'break', 'continue', 'struct', 'class', 'extends',
        'this', 'import', 'macro', 'public', 'private', 'true', 'false', 'typeof',
        'sizeof', 'alignof', 'new', 'null', 'try', 'catch', 'finally', 'throw'
    ];

    const TON_TYPES = [
        'int', 'float', 'bool', 'string', 'void', 'char'
    ];

    const provider = vscode.languages.registerCompletionItemProvider('ton', {
        provideCompletionItems(document, position, token, context) {
            const completionItems = [];

            TON_KEYWORDS.forEach(keyword => {
                const item = new vscode.CompletionItem(keyword, vscode.CompletionItemKind.Keyword);
                completionItems.push(item);
            });

            TON_TYPES.forEach(type => {
                const item = new vscode.CompletionItem(type, vscode.CompletionItemKind.TypeParameter);
                completionItems.push(item);
            });

            return completionItems;
        }
    });

    context.subscriptions.push(provider);
}

function deactivate() {}

module.exports = {
    activate,
    deactivate
}