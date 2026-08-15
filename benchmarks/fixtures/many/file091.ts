interface Item91 { id: number; name: string; }
type Key91 = string | number;
export function make91(id: number, name: string): Item91 {
    const value: Item91 = { id, name };
    return value as Item91;
}
export const item91: Item91 = make91(91, "item-91");
