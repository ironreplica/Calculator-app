// main.mm : macOS (Cocoa) frontend for the calculator.
// Mirrors the Win32 frontend in Calculator_app.cpp / calculator_functionality.cpp
// and uses the same ExpressionTree for the actual math.

#import <Cocoa/Cocoa.h>
#include "../ExpressionTree.h"
#include <string>

static const int BUTTON_COUNT = 26;
static const CGFloat buttonWidth = 90;
static const CGFloat buttonHeight = 44;
static const CGFloat textBoxHeight = 50;
static NSString* const buttons[BUTTON_COUNT] = {
    @"%", @"CE", @"C", @"DEL",
    @"1/x", @"x²", @"√", @"÷",
    @"7", @"8", @"9", @"×",
    @"4", @"5", @"6", @"-",
    @"1", @"2", @"3", @"+",
    @"±", @"0", @".", @"=",
    @"(", @")"
};

@interface AppDelegate : NSObject <NSApplicationDelegate>
@property (strong) NSWindow* window;
@property (strong) NSTextField* inputBox;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification*)notification {
    int rows = (BUTTON_COUNT + 3) / 4;
    NSRect frame = NSMakeRect(0, 0, buttonWidth * 4, textBoxHeight + rows * buttonHeight);
    self.window = [[NSWindow alloc] initWithContentRect:frame
                                              styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable
                                                backing:NSBackingStoreBuffered
                                                  defer:NO];
    self.window.title = @"Calculator app";
    NSView* content = self.window.contentView;

    // Input box across the top (Cocoa's origin is bottom-left)
    self.inputBox = [[NSTextField alloc] initWithFrame:NSMakeRect(0, frame.size.height - textBoxHeight, buttonWidth * 4, textBoxHeight)];
    self.inputBox.font = [NSFont monospacedDigitSystemFontOfSize:24 weight:NSFontWeightRegular];
    self.inputBox.alignment = NSTextAlignmentRight;
    self.inputBox.target = self;
    self.inputBox.action = @selector(returnPressed:); // Return key computes
    [content addSubview:self.inputBox];

    for (int i = 0; i < BUTTON_COUNT; ++i) {
        CGFloat x = buttonWidth * (i % 4);
        CGFloat y = frame.size.height - textBoxHeight - (i / 4 + 1) * buttonHeight;
        NSButton* button = [NSButton buttonWithTitle:buttons[i] target:self action:@selector(buttonPressed:)];
        button.bezelStyle = NSBezelStyleSmallSquare;
        button.frame = NSMakeRect(x, y, buttonWidth, buttonHeight);
        button.font = [NSFont systemFontOfSize:18];
        button.tag = i;
        [content addSubview:button];
    }

    [self.window center];
    [self.window makeKeyAndOrderFront:nil];
    [NSApp activateIgnoringOtherApps:YES];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender {
    return YES;
}

- (void)returnPressed:(id)sender {
    [self compute:self.inputBox.stringValue];
}

- (void)buttonPressed:(NSButton*)sender {
    [self insertChar:buttons[sender.tag]];
}

/**
* @brief Inserts a char into the text box (same rules as the Win32 InsertChar).
*/
- (void)insertChar:(NSString*)character {
    NSString* text = self.inputBox.stringValue;

    if ([character isEqualToString:@"CE"] || [character isEqualToString:@"C"]) {
        self.inputBox.stringValue = @"";
        return;
    }
    if ([character isEqualToString:@"DEL"]) {
        if (text.length > 0) {
            NSRange last = [text rangeOfComposedCharacterSequenceAtIndex:text.length - 1];
            self.inputBox.stringValue = [text substringToIndex:last.location];
        }
        return;
    }
    if ([character isEqualToString:@"="]) {
        [self compute:text];
        return;
    }
    if ([character isEqualToString:@"±"]) {
        [self positiveNegative];
        return;
    }

    // Map display symbols to what the expression tree understands
    NSString* toAppend;
    if ([character isEqualToString:@"x²"])      toAppend = @"^";
    else if ([character isEqualToString:@"×"])  toAppend = @"*";
    else if ([character isEqualToString:@"÷"])  toAppend = @"/";
    else toAppend = [character substringWithRange:[character rangeOfComposedCharacterSequenceAtIndex:0]];

    self.inputBox.stringValue = [text stringByAppendingString:toAppend];
}

/**
*  @brief Toggles the sign of the last number in the expression.
*/
- (void)positiveNegative {
    NSMutableString* expr = [self.inputBox.stringValue mutableCopy];
    NSInteger i = (NSInteger)expr.length - 1;

    // skip trailing whitespace
    while (i >= 0 && [[NSCharacterSet whitespaceCharacterSet] characterIsMember:[expr characterAtIndex:i]])
        --i;
    if (i < 0)
        return;

    // move left over the digits of the last number
    while (i >= 0 && ([[NSCharacterSet decimalDigitCharacterSet] characterIsMember:[expr characterAtIndex:i]] || [expr characterAtIndex:i] == '.'))
        --i;

    NSInteger numberStart = i + 1;
    if (numberStart >= (NSInteger)expr.length)
        return; // no number found

    NSString* lastNumber = [expr substringFromIndex:numberStart];
    if ([lastNumber isEqualToString:@"0"])
        return; // don't toggle 0 to -0

    if (i >= 0 && [expr characterAtIndex:i] == '-')
        [expr deleteCharactersInRange:NSMakeRange(i, 1)];
    else
        [expr insertString:@"-" atIndex:numberStart];

    self.inputBox.stringValue = expr;
}

- (void)showSyntaxError {
    NSAlert* alert = [[NSAlert alloc] init];
    alert.messageText = @"Syntax error";
    alert.alertStyle = NSAlertStyleCritical;
    [alert runModal];
    self.inputBox.stringValue = @"SYNTAX ERROR";
}

/**
* @brief Validates parentheses, evaluates the expression and shows the result.
*/
- (void)compute:(NSString*)expression {
    std::string expressionString = expression.UTF8String;

    // Skip empty input; the expression tree can't handle it
    if (expressionString.find_first_not_of(' ') == std::string::npos)
        return;

    int depth = 0;
    for (char c : expressionString) {
        if (c == '(') {
            ++depth;
        }
        else if (c == ')') {
            if (depth == 0) {
                [self showSyntaxError];
                return;
            }
            --depth;
        }
    }
    if (depth != 0) {
        [self showSyntaxError];
        return;
    }

    ExpressionTree exp(expressionString);
    self.inputBox.stringValue = [NSString stringWithFormat:@"%f", exp.Evaluate()];
}

@end

int main(int argc, const char* argv[]) {
    @autoreleasepool {
        NSApplication* app = [NSApplication sharedApplication];
        [app setActivationPolicy:NSApplicationActivationPolicyRegular];

        // Minimal menu bar so Cmd+Q and copy/paste work
        NSMenu* menuBar = [[NSMenu alloc] init];
        NSMenuItem* appItem = [[NSMenuItem alloc] init];
        [menuBar addItem:appItem];
        NSMenu* appMenu = [[NSMenu alloc] init];
        [appMenu addItemWithTitle:@"Quit Calculator" action:@selector(terminate:) keyEquivalent:@"q"];
        appItem.submenu = appMenu;

        NSMenuItem* editItem = [[NSMenuItem alloc] init];
        [menuBar addItem:editItem];
        NSMenu* editMenu = [[NSMenu alloc] initWithTitle:@"Edit"];
        [editMenu addItemWithTitle:@"Cut" action:@selector(cut:) keyEquivalent:@"x"];
        [editMenu addItemWithTitle:@"Copy" action:@selector(copy:) keyEquivalent:@"c"];
        [editMenu addItemWithTitle:@"Paste" action:@selector(paste:) keyEquivalent:@"v"];
        [editMenu addItemWithTitle:@"Select All" action:@selector(selectAll:) keyEquivalent:@"a"];
        editItem.submenu = editMenu;
        app.mainMenu = menuBar;

        AppDelegate* delegate = [[AppDelegate alloc] init];
        app.delegate = delegate;
        [app run];
    }
    return 0;
}
