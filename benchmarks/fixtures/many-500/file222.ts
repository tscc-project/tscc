interface Item222 { id: number; name: string; }
type Key222 = string | number;
export function make222(id: number, name: string): Item222 {
  const value: Item222 = {id, name};
  return value as Item222;
}
export const item222: Item222 = make222(222, "item-222");
