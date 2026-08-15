interface Item269 { id: number; name: string; }
type Key269 = string | number;
export function make269(id: number, name: string): Item269 {
  const value: Item269 = {id, name};
  return value as Item269;
}
export const item269: Item269 = make269(269, "item-269");
