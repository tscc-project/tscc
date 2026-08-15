interface Item72 { id: number; name: string; }
type Key72 = string | number;
export function make72(id: number, name: string): Item72 {
    const value: Item72 = { id, name };
    return value as Item72;
}
export const item72: Item72 = make72(72, "item-72");
