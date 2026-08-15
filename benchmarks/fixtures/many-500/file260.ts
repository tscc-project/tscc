interface Item260 { id: number; name: string; }
type Key260 = string | number;
export function make260(id: number, name: string): Item260 {
  const value: Item260 = {id, name};
  return value as Item260;
}
export const item260: Item260 = make260(260, "item-260");
