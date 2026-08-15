interface Item59 { id: number; name: string; }
type Key59 = string | number;
export function make59(id: number, name: string): Item59 {
    const value: Item59 = { id, name };
    return value as Item59;
}
export const item59: Item59 = make59(59, "item-59");
