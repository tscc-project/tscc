interface Item193 { id: number; name: string; }
type Key193 = string | number;
export function make193(id: number, name: string): Item193 {
  const value: Item193 = {id, name};
  return value as Item193;
}
export const item193: Item193 = make193(193, "item-193");
