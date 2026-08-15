interface Item40 { id: number; name: string; }
type Key40 = string | number;
export function make40(id: number, name: string): Item40 {
    const value: Item40 = { id, name };
    return value as Item40;
}
export const item40: Item40 = make40(40, "item-40");
